# Architecture

## Goals

1. One reference implementation of the mcRPC wire protocol.
2. Zero knowledge of radios, MeshCore, BLE, MQTT, UART inside the library.
3. Stable C++ API for firmware, desktop, and FFI bindings.
4. Official compliance tests reusable by other implementations.

## Layers

```
┌────────────────────────────────────────────┐
│  Apps: HA · CLI · Android · firmware app   │
├────────────────────────────────────────────┤
│  Public API  include/mcrpc/*.h             │
│  Parser · Builders · Dispatcher · McRpc    │
│  Feature SDK · HostServices                │
├────────────────────────────────────────────┤
│  Transport adapter (OUT OF TREE)           │
│  MeshCore GRP_TXT · MQTT · UART · sim      │
└────────────────────────────────────────────┘
```

## Public vs internal

| Public (`include/mcrpc`) | Internal (`src/*.cpp`) |
|--------------------------|-------------------------|
| All installed headers    | `.cpp` translation units |
| Feature headers          | Feature static `g_*` handlers |
| `FeatureManager` (needed to `add()` features) | Implementation bodies |

There is no second “internal header tree” today: anything an application needs
to compose a node is public. Implementation details stay in `.cpp` files.

## HostServices

Features call virtual methods on `HostServices`. The host (firmware adapter or
simulator) implements battery/GPS/button/display. This is the hardware boundary.

## ConfigStore

Persistence is injected. The library never opens files or flash.

## PublishFn

Outbound path is a single callback: `bool (*)(const char* text, void*)`.
Transports wrap text into their packet format.
