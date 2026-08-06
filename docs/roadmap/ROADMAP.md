# Roadmap

## 1.0.0 (current)

Standalone library, stable protocol and SDK, compliance + golden suites, CLI, documentation.

## 1.1

- Protocol evolution (additive): see
  [`RFC-0001`](../rfc/RFC-0001-mcrpc-1.1.md) +
  [`implementation plan`](../rfc/IMPLEMENTATION_PLAN-RFC-0001.md) —
  architecture frozen; **not coded until plan execution**
- Thin **C ABI** (`bindings/c`) for FFI
- Python ctypes/cffi wrapper (parse/build)
- Optional CMake target `mcrpc_core` without reference feature objects
- Expanded golden corpus; optional nightly fuzz job

## 1.x later

- Kotlin/JNI and Go cgo packages
- Out-of-tree Home Assistant custom component using the C ABI

## 2.0

Only if additive evolution is impossible (API removals or protocol major bump).

### Non-goals

- Binary TLV wire format
- Embedding MQTT/HTTP/MeshCore inside libmcrpc
