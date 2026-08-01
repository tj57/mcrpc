# API Reference (SDK 1.0)

Include `<mcrpc/mcrpc.h>` for applications or `<mcrpc/FeatureSdk.h>` for features.

Full detail: headers under `include/mcrpc/` are the source of truth.

## Versions

```cpp
mcrpc::protocolVersionString();  // "1.0"
mcrpc::sdkVersionString();       // "1.0.0"
mcrpc::libraryVersionString();   // "1.0.0"
```

Discover lines include `protocol=` and `sdk=`.

## Core

| Type | Role |
|------|------|
| `Parser` | Parse / strip chat prefix |
| `Dispatcher` | Address filter + command dispatch |
| `OutboundBuilder` | request / event / err / ok |
| `StatusBuilder` / `DiscoverBuilder` | Structured replies |
| `CommandRegistry` / `CapabilityRegistry` | Tables |
| `EventBus` | Async events |
| `Config` / `ConfigStore` | Prefs + persistence interface |
| `McRpc` | Facade |
| `HostServices` | Host callbacks |
| `Feature` / `FeatureManager` | Feature SDK |

## Stability

Breaking changes require SDK major bump and CHANGELOG migration notes.
Additive APIs are allowed in 1.x.
