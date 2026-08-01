# SDK Guide — adding features

```cpp
#include <mcrpc/FeatureSdk.h>

class TempFeature : public mcrpc::Feature {
public:
  explicit TempFeature(mcrpc::HostServices& host) : _host(host) {}
  const char* name() const override { return "temp"; }
  void registerCommands(mcrpc::CommandRegistry& c) override {
    c.registerCommand("temp", &TempFeature::cmd, "temperature");
  }
  void registerCapabilities(mcrpc::CapabilityRegistry& caps) override {
    caps.registerCapability("temperature");
  }
private:
  mcrpc::HostServices& _host;
  static bool cmd(mcrpc::CommandContext& ctx);
};
```

Lifecycle: `setup` → `registerCommands` → `registerCapabilities` → `loop*` → `shutdown`.

Contribute status/discover fields via `collectStatus` / `collectDiscover`.
