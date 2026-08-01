# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/).
This project uses independent **protocol**, **SDK**, and **library** versions
(see `include/mcrpc/Version.h`).

## [1.0.0] — 2026-08-01

First public release of standalone **mcRPC**.

### Highlights

- Transport-independent C++ reference implementation of protocol **1.0**
- Stable public SDK **1.0.0** under `include/mcrpc/`
- Feature SDK with reference features (core, battery, button, GPS, relay, display)
- Official compliance + golden test suites (interop contract)
- First-class CLI: `parse`, `build`, `encode`, `decode`, `validate`, `discover`, `ping`, `version`
- Desktop examples with zero embedded SDK dependency
- CMake + PlatformIO (`library.json`) packaging

### Protocol

- Human-readable text RPC (`target[#id] command [args…]`)
- Discover fields include `protocol=` and `sdk=`
- `err unknown_command` vs `err unsupported` semantics documented
- Chat-style `Name: ` prefix stripping requires whitespace after `:` (preserves `group:name`)

### Documentation

- Architecture, transport adapter guide, API, SDK guide
- Normative protocol specification
- Compatibility statement, migration guide, Home Assistant design notes
- Release checklist and project state

### Not included in 1.0

- Language bindings (C / Python / Kotlin / Go) — design stubs only
- Home Assistant custom component
- MeshCore transport adapter (lives in the MeshCore repository)

### Upgrade notes

If you previously used mcRPC embedded under MeshCore `lib/mcrpc`:

1. Depend on this repository (submodule / PlatformIO lib / package).
2. Keep using MeshCore `examples/mcrpc` as the transport adapter.
3. Use `Config::begin(ConfigStore*)` only (`begin(void*)` was removed).

[1.0.0]: https://github.com/mcrpc/mcrpc/releases/tag/v1.0.0
