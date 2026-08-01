/* Integration: fake transport + McRpc end-to-end. */
#include <mcrpc/mcrpc.h>
#include <mcrpc/features/core/CoreFeature.h>
#include <cstdio>
#include <cstring>

using namespace mcrpc;

static int g_fail = 0, g_pass = 0;
#define EXPECT(...) do { \
  if (!(__VA_ARGS__)) { std::printf("FAIL %s:%d\n", __FILE__, __LINE__); g_fail++; } \
  else g_pass++; \
} while (0)

static char g_pub[256];
static bool pub(const char* t, void*) {
  std::snprintf(g_pub, sizeof(g_pub), "%s", t);
  return true;
}

struct Host : HostServices {
  const char* nodeName() override { return "n1"; }
  const char* profile() override { return "p"; }
  const char* firmwareVersion() override { return "f"; }
  uint32_t uptimeSeconds() override { return 9; }
};

int main() {
  McRpc rpc;
  Host host;
  host.engine = &rpc;
  CoreFeature core(host);
  rpc.features().add(&core);
  rpc.setPublishHandler(pub, nullptr);
  rpc.setNodeIdentity("n1", "g1");
  rpc.setProfile("p");
  rpc.setFirmwareVersion("f");
  rpc.begin();

  EXPECT(rpc.handleIncomingText("Alice: n1 ping") == true);
  EXPECT(std::strcmp(g_pub, "pong") == 0);

  EXPECT(rpc.handleIncomingText("group:g1 ping") == true);
  EXPECT(std::strcmp(g_pub, "pong") == 0);

  EXPECT(rpc.handleIncomingText("other ping") == false);

  rpc.shutdown();
  if (g_fail) {
    std::printf("INTEGRATION FAIL %d\n", g_fail);
    return 1;
  }
  std::printf("INTEGRATION OK (%d)\n", g_pass);
  return 0;
}
