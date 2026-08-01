# Remaining MeshCore dependencies

The standalone **mcRPC library has zero compile-time dependencies on MeshCore**.

The following exist only in the **MeshCore consumer** repository:

| Component | Path (MeshCore) | Why it stays |
|-----------|-----------------|--------------|
| GRP_TXT transport | `examples/mcrpc/McRpcMesh.*` | MeshCore packet/channel APIs |
| Arduino FS config | `examples/mcrpc/ArduinoFsConfigStore.h` | SPIFFS/LittleFS/InternalFS |
| GPS power helper | `examples/mcrpc/drivers/OnDemandGps.h` | Board UART / EnvironmentSensorManager |
| Firmware entry | `examples/mcrpc/main.cpp` | Arduino setup/loop, board, radio |
| Board envs | `variants/*/platformio.ini` (`*_mcrpc_*`) | PlatformIO / BSP |
| Channel docs | `doc/PRIVATE_CHANNELS.md` etc. | Mesh deployment guidance |

Optional future: MeshCore may vendor `lib/mcrpc` as a git submodule pointing at this repo.
