#include <mcrpc/mcrpc.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace mcrpc;
namespace fs = std::filesystem;

#ifndef MCRPC_GOLDEN_DIR
#define MCRPC_GOLDEN_DIR "tests/golden/cases"
#endif

static int g_fail = 0;
static int g_pass = 0;

static bool hPing(CommandContext& ctx) {
  ctx.reply->clear();
  ctx.reply->append("pong");
  return true;
}

static bool hUnsupported(CommandContext& ctx) {
  ctx.reply->clear();
  ctx.reply->append("err unsupported");
  return true;
}

struct Case {
  std::string in;
  std::string out;
  std::string mode;  // dispatch | ignore | build_event | build_request | build_discover | build_status
};

static std::string trim(std::string s) {
  while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ')) s.pop_back();
  size_t i = 0;
  while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) i++;
  return s.substr(i);
}

static std::vector<Case> loadFile(const fs::path& path) {
  std::ifstream in(path);
  std::vector<Case> cases;
  Case cur;
  std::string line;
  enum { None, WantIn, WantOut, WantMode } st = None;
  auto flush = [&]() {
    if (!cur.in.empty() || !cur.out.empty()) {
      if (cur.mode.empty()) cur.mode = "dispatch";
      cases.push_back(cur);
    }
    cur = Case{};
    st = None;
  };
  while (std::getline(in, line)) {
    // Comments: "# …" (hash + whitespace). Request-id bodies like "#18 pong" are data.
    if (!line.empty() && line[0] == '#' &&
        (line.size() == 1 || line[1] == ' ' || line[1] == '\t'))
      continue;
    std::string t = trim(line);
    if (t == "---") {
      flush();
      continue;
    }
    if (t == "IN") {
      st = WantIn;
      continue;
    }
    if (t == "OUT") {
      st = WantOut;
      continue;
    }
    if (t == "MODE") {
      st = WantMode;
      continue;
    }
    if (st == WantIn) {
      cur.in = t;
      st = None;
    } else if (st == WantOut) {
      cur.out = t;
      st = None;
    } else if (st == WantMode) {
      cur.mode = t;
      st = None;
    }
  }
  flush();
  return cases;
}

static void check(const char* file, const Case& c, const std::string& got, bool expect_reply) {
  if (!expect_reply) {
    if (c.out == "-" || c.out.empty()) {
      g_pass++;
      return;
    }
  }
  if (got == c.out) {
    g_pass++;
    return;
  }
  // soft prefix match for status (uptime/rssi may vary)
  if (c.mode == "build_status" && got.rfind(c.out, 0) == 0) {
    g_pass++;
    return;
  }
  std::printf("FAIL %s\n  IN:  %s\n  EXP: %s\n  GOT: %s\n", file, c.in.c_str(), c.out.c_str(),
              got.c_str());
  g_fail++;
}

static void runCase(const char* file, const Case& c, Dispatcher& disp) {
  ReplyBuffer buf;
  if (c.mode == "dispatch" || c.mode == "ignore") {
    bool replied = disp.dispatch(c.in.c_str(), buf);
    if (c.mode == "ignore") {
      if (!replied) {
        g_pass++;
        return;
      }
      std::printf("FAIL %s expected no reply, got '%s'\n", file, buf.data);
      g_fail++;
      return;
    }
    check(file, c, replied ? std::string(buf.data) : std::string(), true);
    return;
  }
  if (c.mode == "build_event") {
    // IN is "name [kv]"
    std::string name = c.in;
    std::string kv;
    auto sp = name.find(' ');
    if (sp != std::string::npos) {
      kv = name.substr(sp + 1);
      name = name.substr(0, sp);
    }
    OutboundBuilder::event(buf, name.c_str(), kv.empty() ? nullptr : kv.c_str());
    check(file, c, buf.data, true);
    return;
  }
  if (c.mode == "build_request") {
    // IN like tracker#42 status or tracker gps
    Request r;
    if (Parser::parse(c.in.c_str(), r) != ParseResult::Ok) {
      std::printf("FAIL %s parse for build_request\n", file);
      g_fail++;
      return;
    }
    OutboundBuilder::request(buf, r.target, r.command, r.has_request_id, r.request_id);
    check(file, c, buf.data, true);
    return;
  }
  if (c.mode == "build_discover") {
    DiscoverBuilder d;
    d.setNodeName("tracker");
    d.add("id", "3cbbf74e");
    d.add("fw", "test");
    d.add("v", protocolVersionString());
    d.add("tag", "tracker");
    d.add("up", "42s");
    d.add("caps", "battery,button");
    d.writeTo(buf);
    check(file, c, buf.data, true);
    return;
  }
  if (c.mode == "build_status") {
    StatusBuilder s;
    s.add("name", "tracker");
    s.add("id", "3cbbf74e");
    s.add("fw", "test");
    s.add("v", protocolVersionString());
    s.add("tag", "tracker");
    s.add("transport", "meshcore");
    s.writeTo(buf);
    check(file, c, buf.data, true);
    return;
  }
  std::printf("FAIL %s unknown MODE %s\n", file, c.mode.c_str());
  g_fail++;
}

static bool hCall(CommandContext& ctx) {
  if (ctx.request->argc < 1) {
    CallResult::err(*ctx.reply, "invalid_argument");
    return true;
  }
  const char* proc = ctx.request->args[0];
  if (!CallResult::isValidProc(proc)) {
    CallResult::err(*ctx.reply, "invalid_argument");
    CallResult::appendKv(*ctx.reply, "reason", "proc");
    return true;
  }
  if (std::strcmp(proc, "scene.nope") == 0) {
    CallResult::err(*ctx.reply, "unknown_proc");
    return true;
  }
  if (std::strcmp(proc, "button.pressed") == 0) {
    CallResult::ok(*ctx.reply);
    CallResult::appendKv(*ctx.reply, "lat", "50");
    CallResult::appendKv(*ctx.reply, "lon", "19");
    return true;
  }
  CallResult::ok(*ctx.reply);
  return true;
}

int main() {
  CommandRegistry reg;
  reg.registerCommand("ping", hPing);
  reg.registerCommand("call", hCall);
  // Registered features that are unavailable on this device (SPEC §18).
  reg.registerCommand("relay", hUnsupported);
  reg.registerCommand("battery", hUnsupported);
  reg.registerCommand("gps", hUnsupported);
  Dispatcher disp(reg);
  disp.setNodeName("tracker");
  disp.setGroupName("mych");

  fs::path dir = MCRPC_GOLDEN_DIR;
  if (!fs::exists(dir)) {
    std::printf("golden dir missing: %s\n", dir.c_str());
    return 1;
  }
  for (auto& ent : fs::directory_iterator(dir)) {
    if (ent.path().extension() != ".txt") continue;
    auto cases = loadFile(ent.path());
    for (auto& c : cases) runCase(ent.path().filename().c_str(), c, disp);
  }

  if (g_fail) {
    std::printf("GOLDEN: %d FAILED, %d passed\n", g_fail, g_pass);
    return 1;
  }
  std::printf("GOLDEN OK (%d assertions)\n", g_pass);
  return 0;
}
