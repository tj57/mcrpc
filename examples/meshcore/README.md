# MeshCore adapter example

The MeshCore transport adapter is **not** part of this library.

It lives in the MeshCore repository:

```
MeshCore/examples/mcrpc/
  McRpcMesh.h|.cpp          # GRP_TXT ↔ PublishFn / handleInbound
  ArduinoFsConfigStore.h    # ConfigStore over board FS
  drivers/OnDemandGps.h     # board GPS power helper
  main.cpp                  # Arduino entry
```

## How MeshCore consumes mcRPC

1. Depend on this library (`lib_deps = mcrpc` or git submodule / `lib_extra_dirs`).
2. Implement `HostServices` (McRpcMesh does).
3. On inbound group text: `Parser::stripSenderPrefix` → `McRpc::handleIncomingText`.
4. On outbound: `setPublishHandler` sends channel text via MeshCore packets.

See [docs/development/MIGRATION_MESHCORE.md](../../docs/development/MIGRATION_MESHCORE.md)
and [docs/architecture/TRANSPORT.md](../../docs/architecture/TRANSPORT.md).
