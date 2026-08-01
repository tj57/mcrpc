/**
 * Firmware-style feature composition without any board SDK.
 * Shows HostServices injection pattern used by real firmware adapters.
 */
#include <mcrpc/mcrpc.h>
#include <mcrpc/features/core/CoreFeature.h>
#include <mcrpc/features/battery/BatteryFeature.h>
#include <cstdio>
#include <cstring>

using namespace mcrpc;

struct StubHost : HostServices {
  const char* nodeName() override { return "node1"; }
  const char* profile() override { return "sensor"; }
  const char* firmwareVersion() override { return "fw-stub"; }
  uint32_t uptimeSeconds() override { return 42; }
  bool readBattery(float& v, int& p) override {
    v = 3.9f;
    p = 88;
    return true;
  }
};

int main() {
  McRpc rpc;
  StubHost host;
  host.engine = &rpc;
  CoreFeature core(host);
  BatteryFeature batt(host);
  rpc.features().add(&core);
  rpc.features().add(&batt);
  rpc.setNodeIdentity("node1", nullptr);
  rpc.setProfile("sensor");
  rpc.setFirmwareVersion("fw-stub");
  rpc.begin();

  ReplyBuffer reply;
  rpc.dispatcher().dispatch("node1 battery", reply);
  std::printf("%s\n", reply.data);
  rpc.shutdown();
  return std::strstr(reply.data, "battery") ? 0 : 1;
}
