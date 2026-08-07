/**
 * Desktop mini-node: dispatch ping/status over a fake publish callback.
 */
#include <mcrpc/mcrpc.h>
#include <mcrpc/features/core/CoreFeature.h>
#include <cstdio>
#include <cstring>

using namespace mcrpc;

static char g_last[256];

static bool publish(const char* text, void*) {
  std::snprintf(g_last, sizeof(g_last), "%s", text);
  std::printf("<< %s\n", text);
  return true;
}

struct DesktopHost : HostServices {
  const char* nodeName() override { return "desktop"; }
  const char* profile() override { return "desktop"; }
  const char* firmwareVersion() override { return "example"; }
  uint32_t uptimeSeconds() override { return 1; }
};

int main() {
  McRpc rpc;
  DesktopHost host;
  host.engine = &rpc;
  CoreFeature core(host);
  rpc.features().add(&core);
  rpc.setPublishHandler(publish, nullptr);
  rpc.setNodeIdentity("desktop", "lab");
  rpc.setProfile("desktop");
  rpc.setFirmwareVersion("example");
  rpc.begin();

  std::printf(">> desktop ping\n");
  rpc.handleIncomingText("desktop ping");
  if (std::strcmp(g_last, "pong") != 0) return 1;

  std::printf(">> all discovery\n");
  rpc.handleIncomingText("all discovery");
  // RFC-0002 slim discovery: wire version is ``v=``, not ``protocol=``.
  if (!std::strstr(g_last, "v=1.2")) return 1;
  if (std::strstr(g_last, "protocol=")) return 1;

  rpc.shutdown();
  return 0;
}
