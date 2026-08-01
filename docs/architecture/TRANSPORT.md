# Transport adapter guide

## Contract

mcRPC never includes transport headers. An adapter must:

1. **Inbound:** receive a text payload → optional chat prefix strip →
   `McRpc::handleIncomingText` or `Dispatcher::dispatch`.
2. **Outbound:** register `setPublishHandler` (or send `ReplyBuffer` manually).
3. **Identity:** `setNodeIdentity(node, group)` for addressing.
4. **Hardware:** implement `HostServices` if using reference features.

## Minimal adapter sketch

```cpp
#include <mcrpc/mcrpc.h>

static MyRadio* radio;

static bool publish(const char* text, void*) {
  return radio->sendChannelText(text);
}

void onChannelText(const char* text) {
  rpc.handleIncomingText(text);
}

void setupRpc(mcrpc::McRpc& rpc) {
  rpc.setPublishHandler(publish, nullptr);
  rpc.setNodeIdentity("tracker", "mych");
  rpc.begin();
}
```

## What adapters own

| Concern | Owner |
|---------|-------|
| Encryption / channel keys | Transport |
| Packet framing | Transport |
| Retry / ACK | Transport |
| `Name: ` chat prefixes | Transport or `stripSenderPrefix` |
| Pin I/O, GPS power | HostServices impl |
| mcRPC grammar | **libmcrpc only** |

## MeshCore

See `examples/meshcore/README.md` and MeshCore `examples/mcrpc/`.

## Non-goals

Do not add MQTT, BLE, or UART clients inside this repository.
