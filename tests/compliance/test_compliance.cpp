/* Protocol compliance suite — shared contract for all mcRPC implementations. */
#include <mcrpc/mcrpc.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

using namespace mcrpc;

static int g_fail = 0;
static int g_pass = 0;

#define EXPECT(...) do { \
  if (!(__VA_ARGS__)) { \
    std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #__VA_ARGS__); \
    g_fail++; \
  } else { g_pass++; } \
} while (0)

static bool hPing(CommandContext& ctx) {
  ctx.reply->clear();
  ctx.reply->append("pong");
  return true;
}

static bool hStatus(CommandContext& ctx) {
  ctx.reply->clear();
  ctx.reply->append("status name=n tag=p fw=f up=1s rssi=0");
  return true;
}

static bool hUnsupported(CommandContext& ctx) {
  ctx.reply->clear();
  ctx.reply->append("err unsupported");
  return true;
}

static bool hCall(CommandContext& ctx) {
  if (ctx.request->argc < 1 || !CallResult::isValidProc(ctx.request->args[0])) {
    CallResult::err(*ctx.reply, "invalid_argument");
    return true;
  }
  CallResult::ok(*ctx.reply);
  CallResult::appendKv(*ctx.reply, "state", "closed");
  return true;
}

/* ---------- Parser ---------- */

static void test_whitespace() {
  Request r;
  EXPECT(Parser::parse("  tracker   ping  ", r) == ParseResult::Ok);
  EXPECT(std::strcmp(r.command, "ping") == 0);
  EXPECT(Parser::parse("\ttracker\tping\t", r) == ParseResult::Ok);
}

static void test_request_ids() {
  Request r;
  EXPECT(Parser::parse("ha#42 ping", r) == ParseResult::Ok);
  EXPECT(r.has_request_id && r.request_id == 42);
  EXPECT(Parser::parse("ha #99 status", r) == ParseResult::Ok);
  EXPECT(r.has_request_id && r.request_id == 99);
}

static void test_addressing() {
  Request r;
  EXPECT(Parser::parse("all ping", r) == ParseResult::Ok);
  EXPECT(r.address_kind == AddressKind::All);
  EXPECT(Parser::parse("self ping", r) == ParseResult::Ok);
  EXPECT(r.address_kind == AddressKind::Self);
  EXPECT(Parser::parse("group:sensors ping", r) == ParseResult::Ok);
  EXPECT(r.address_kind == AddressKind::Group);
  EXPECT(std::strcmp(r.target, "sensors") == 0);
}

static void test_malformed() {
  Request r;
  EXPECT(Parser::parse("", r) == ParseResult::Empty);
  EXPECT(Parser::parse("tracker", r) == ParseResult::MissingCommand);
  EXPECT(Parser::parse(nullptr, r) == ParseResult::Empty);
}

static void test_case() {
  Request r;
  EXPECT(Parser::parse("N Ping", r) == ParseResult::Ok);
  EXPECT(std::strcmp(r.command, "ping") == 0);
  EXPECT(Parser::parse("n set Led ON", r) == ParseResult::Ok);
  EXPECT(std::strcmp(r.args[0], "Led") == 0);
  EXPECT(std::strcmp(r.args[1], "ON") == 0);
}

static void test_call_parser_neutral() {
  Request r;
  EXPECT(Parser::parse("ha call scene.morning", r) == ParseResult::Ok);
  EXPECT(std::strcmp(r.command, "call") == 0);
  EXPECT(r.argc == 1);
  EXPECT(std::strcmp(r.args[0], "scene.morning") == 0);
  EXPECT(CallResult::isValidProc("button.pressed"));
  EXPECT(CallResult::isValidProc("ha.notify"));
  EXPECT(!CallResult::isValidProc("button_pressed"));
  EXPECT(!CallResult::isValidProc("button.press.v2"));
  EXPECT(!CallResult::isValidProc(".pressed"));
}

/* ---------- Dispatcher / errors / broadcast ---------- */

static void test_dispatch_errors_broadcast() {
  CommandRegistry reg;
  reg.registerCommand("ping", hPing);
  reg.registerCommand("status", hStatus);
  reg.registerCommand("relay", hUnsupported);
  reg.registerCommand("battery", hUnsupported);
  reg.registerCommand("gps", hUnsupported);
  reg.registerCommand("call", hCall);
  Dispatcher d(reg);
  d.setNodeName("tracker");
  d.setGroupName("mych");
  ReplyBuffer reply;

  EXPECT(d.dispatch("all ping", reply) == true);
  EXPECT(std::strcmp(reply.data, "pong") == 0);

  EXPECT(d.dispatch("other ping", reply) == false);

  EXPECT(d.dispatch("tracker nope", reply) == true);
  EXPECT(std::strstr(reply.data, "unknown_command") != nullptr);

  EXPECT(d.dispatch("tracker relay", reply) == true);
  EXPECT(std::strstr(reply.data, "unsupported") != nullptr);
  EXPECT(std::strstr(reply.data, "unknown_command") == nullptr);

  EXPECT(d.dispatch("tracker battery", reply) == true);
  EXPECT(std::strstr(reply.data, "unsupported") != nullptr);
  EXPECT(d.dispatch("tracker gps", reply) == true);
  EXPECT(std::strstr(reply.data, "unsupported") != nullptr);

  EXPECT(d.dispatch("tracker#3 ping", reply) == true);
  EXPECT(std::strcmp(reply.data, "#3 pong") == 0);

  EXPECT(d.dispatch("group:mych ping", reply) == true);
  EXPECT(d.dispatch("group:x ping", reply) == false);

  EXPECT(d.dispatch("tracker#7 call button.pressed", reply) == true);
  EXPECT(std::strcmp(reply.data, "#7 ok state=closed") == 0);
}

/* ---------- Builders / events / caps ---------- */

static void test_builders_and_events() {
  StatusBuilder s;
  s.add("name", "n");
  s.add("battery", 50);
  ReplyBuffer r;
  s.writeTo(r);
  EXPECT(std::strstr(r.data, "status ") != nullptr);
  EXPECT(std::strstr(r.data, "battery=50") != nullptr);

  DiscoverBuilder d;
  d.setNodeName("tracker");
  d.add("tag", "tracker");
  d.add("fw", "fw");
  d.add("v", protocolVersionString());
  d.writeTo(r);
  EXPECT(std::strstr(r.data, "v=1.2") != nullptr);
  EXPECT(std::strstr(r.data, "tag=tracker") != nullptr);

  OutboundBuilder::event(r, "button.pressed", "count=1");
  EXPECT(std::strcmp(r.data, "event button.pressed count=1") == 0);

  CallResult::err(r, "unsupported");
  CallResult::appendKv(r, "feature", "gps");
  EXPECT(std::strcmp(r.data, "err unsupported feature=gps") == 0);

  CapabilityRegistry caps;
  caps.registerCapability("gps");
  caps.registerCapability("battery");
  caps.writeTo(r);
  EXPECT(std::strstr(r.data, "gps") != nullptr);
  EXPECT(std::strchr(r.data, '\n') != nullptr);
}

/* ---------- Aliases (location → same handler family) ---------- */

static void test_command_alias_registration() {
  CommandRegistry reg;
  reg.registerCommand("gps", hPing);
  reg.registerCommand("location", hPing);
  EXPECT(reg.find("gps") != nullptr);
  EXPECT(reg.find("location") != nullptr);
  EXPECT(reg.find("GPS") != nullptr);
}

/* ---------- Response generation / sender prefix ---------- */

static void test_response_generation() {
  ReplyBuffer r;
  OutboundBuilder::ok(r);
  EXPECT(std::strcmp(r.data, "ok") == 0);
  OutboundBuilder::ok(r, "done");
  EXPECT(std::strcmp(r.data, "ok done") == 0);
  OutboundBuilder::request(r, "tracker", "gps", true, 7);
  EXPECT(std::strcmp(r.data, "tracker#7 gps") == 0);
  const char* set_args[] = {"led", "on"};
  OutboundBuilder::requestWithArgs(r, "n", "set", 2, set_args, false, 0);
  EXPECT(std::strcmp(r.data, "n set led on") == 0);

  const char* stripped = Parser::stripSenderPrefix("Alice: tracker ping");
  EXPECT(std::strcmp(stripped, "tracker ping") == 0);
  EXPECT(std::strcmp(Parser::stripSenderPrefix("tracker ping"), "tracker ping") == 0);
  EXPECT(std::strcmp(Parser::stripSenderPrefix("Home Assistant: all ping"), "all ping") == 0);
  EXPECT(std::strcmp(Parser::stripSenderPrefix("group:sensors ping"), "group:sensors ping") == 0);
  Request gr;
  EXPECT(Parser::parse(Parser::stripSenderPrefix("group:sensors ping"), gr) == ParseResult::Ok);
  EXPECT(gr.address_kind == AddressKind::Group);
}

static void test_uptime_format() {
  char buf[16];
  EXPECT(formatUptime(45, buf, sizeof(buf)));
  EXPECT(std::strcmp(buf, "45s") == 0);
  EXPECT(formatUptime(9209, buf, sizeof(buf)));
  EXPECT(std::strcmp(buf, "2h33m") == 0);
  EXPECT(formatUptime(90000, buf, sizeof(buf)));
  EXPECT(std::strcmp(buf, "1d1h") == 0);
}

/* ---------- Framework discover versions ---------- */

static char g_pub[256];
static bool cap(const char* t, void*) {
  std::snprintf(g_pub, sizeof(g_pub), "%s", t);
  return true;
}

static uint32_t fakeUp(void*) { return 9209; }
static int fakeRssi(void*) { return -91; }

static void test_framework_discover_versions() {
  McRpc rpc;
  class F : public Feature {
  public:
    const char* name() const override { return "f"; }
    void registerCommands(CommandRegistry& c) override { c.registerCommand("ping", hPing); }
    void registerCapabilities(CapabilityRegistry& caps) override {
      caps.registerCapability("button");
    }
  } f;
  rpc.features().add(&f);
  rpc.setPublishHandler(cap, nullptr);
  rpc.setNodeIdentity("tracker", "mych");
  rpc.setNodeId("3CBBF74E1FEEF235A68BAE7E2DBDECB803E046C9C2F6C2AEE710380222AD60FC");
  rpc.setTag("ha");
  rpc.setFirmwareVersion("2.11.0");
  rpc.setIdentityCallbacks(fakeUp, fakeRssi, nullptr);
  rpc.begin();
  DiscoverBuilder d;
  rpc.buildDiscover(d);
  ReplyBuffer r;
  d.writeTo(r);
  EXPECT(std::strstr(r.data, "id=3cbbf74e") != nullptr);
  EXPECT(std::strstr(r.data, "v=1.2") != nullptr);
  EXPECT(std::strstr(r.data, "fw=2.11.0") != nullptr);
  EXPECT(std::strstr(r.data, "tag=ha") != nullptr);
  EXPECT(std::strstr(r.data, "up=2h33m") != nullptr);
  EXPECT(std::strstr(r.data, "caps=button") != nullptr);
  EXPECT(std::strstr(r.data, "protocol=") == nullptr);
  EXPECT(std::strstr(r.data, "sdk=") == nullptr);
  EXPECT(std::strstr(r.data, "features=") == nullptr);
  EXPECT(std::strstr(r.data, "profile=") == nullptr);

  StatusBuilder st;
  rpc.buildStatus(st);
  st.writeTo(r);
  EXPECT(std::strstr(r.data, "id_full=3CBBF74E") != nullptr ||
         std::strstr(r.data, "id_full=3cbbf74e") != nullptr);
  EXPECT(std::strstr(r.data, "transport=meshcore") != nullptr);
  EXPECT(std::strstr(r.data, "rssi=-91") != nullptr);
  rpc.shutdown();
}

int main() {
  EXPECT(std::strcmp(protocolVersionString(), "1.2") == 0);
  EXPECT(std::strcmp(sdkVersionString(), "1.2.0") == 0);

  test_whitespace();
  test_request_ids();
  test_addressing();
  test_malformed();
  test_case();
  test_call_parser_neutral();
  test_dispatch_errors_broadcast();
  test_builders_and_events();
  test_command_alias_registration();
  test_response_generation();
  test_uptime_format();
  test_framework_discover_versions();

  if (g_fail) {
    std::printf("COMPLIANCE: %d FAILED, %d passed\n", g_fail, g_pass);
    return 1;
  }
  std::printf("COMPLIANCE OK (%d assertions)\n", g_pass);
  return 0;
}
