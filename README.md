# mcRPC

**Transport-independent text RPC** for firmware, Home Assistant, mobile apps, CLI tools, and simulators.

[![CI](https://img.shields.io/badge/CI-local%20scripts-blue)](.github/workflows/ci.yml)
![Protocol](https://img.shields.io/badge/protocol-1.0-green)
![SDK](https://img.shields.io/badge/SDK-1.0.0-green)
![License](https://img.shields.io/badge/license-MIT-blue)

```
Application  →  libmcrpc  →  Transport adapter  →  Link
```

mcRPC defines a small, human-readable application protocol (`tracker#18 gps`, `event button_pressed`, …).  
Routing, encryption, and radios belong to the **transport** (MeshCore, MQTT, UART, …)—not this library.

| Version axis | Current |
|--------------|---------|
| Protocol (wire) | **1.0** |
| SDK (C++ API) | **1.0.0** |
| Library (package) | **1.0.0** |

**Status:** 1.0.0 release candidate — API and protocol stable.

## Why it exists

One reference implementation shared by every consumer so parse/build behaviour never forks between firmware, Home Assistant, Android, and desktop tools.

## Supported platforms

| Platform | Support |
|----------|---------|
| Desktop Linux / macOS / Windows (CMake, C++17) | First-class |
| PlatformIO / Arduino-class firmware (via `library.json`) | First-class |
| Home Assistant / Python / Kotlin / Go | Bindings designed; not shipped in 1.0 |

No Arduino, MeshCore, or radio SDK is required to build the library or run tests.

## Quick start

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure

./build/mcrpc version
./build/mcrpc parse "tracker#1 ping"
./build/mcrpc ping tracker
```

```cpp
#include <mcrpc/mcrpc.h>

mcrpc::Request r;
mcrpc::Parser::parse("tracker#18 gps", r);

mcrpc::ReplyBuffer out;
mcrpc::OutboundBuilder::request(out, "tracker", "gps", true, 18);
```

## Building

```bash
# Full verification (tests + examples + CLI + doc links)
./scripts/test/run-all.sh

# Library only
cmake -S . -B build -DMCRPC_BUILD_EXAMPLES=OFF
cmake --build build
```

PlatformIO: add this repository as a library (`lib_deps` / `lib_extra_dirs`). See `library.json`.

## Examples

| Example | Purpose |
|---------|---------|
| `examples/parser` | Parse a request line |
| `examples/builder` | Build request/event lines |
| `examples/desktop` | Mini node with fake publish callback |
| `examples/firmware` | HostServices + features (no board SDK) |
| `examples/homeassistant` | Peer-style parse/build |
| `examples/cli` | Points at the `mcrpc` tool |
| `examples/meshcore` | Docs for the out-of-tree MeshCore adapter |

## Documentation

| Doc | Description |
|-----|-------------|
| [Architecture](docs/architecture/ARCHITECTURE.md) | Layers and boundaries |
| [Layout](docs/architecture/LAYOUT.md) | Repository map |
| [Transport adapter](docs/architecture/TRANSPORT.md) | How to plug in a transport |
| [Protocol specification](docs/protocol/SPEC.md) | Normative wire grammar |
| [Compatibility](docs/protocol/COMPATIBILITY.md) | Interop rules |
| [API reference](docs/api/API.md) | Public C++ API |
| [SDK guide](docs/development/SDK_GUIDE.md) | Adding features |
| [Testing](docs/development/TESTING.md) | Suites and fuzzing |
| [Migration (MeshCore)](docs/development/MIGRATION_MESHCORE.md) | Consuming this library |
| [Home Assistant](docs/homeassistant.md) | Integration design |
| [Roadmap](docs/roadmap/ROADMAP.md) | 1.1 / 2.0 |
| [Release checklist](docs/RELEASE_CHECKLIST.md) | Publish gate |
| [Project state](docs/STATE.md) | Maturity and debt |

## Project layout

```
include/mcrpc/   Public headers
src/             Implementation
tests/           Unit · compliance · golden · integration · fuzz
examples/        Desktop-compilable demos
tools/mcrpc_cli/ First-class CLI
docs/            Architecture · protocol · API · development
bindings/        Future FFI designs (not implemented)
```

## Roadmap (short)

- **1.0.0** — Stable protocol + SDK (this release)
- **1.1** — C ABI, Python wrapper, optional core-only CMake target
- **2.0** — Only if breaking changes become unavoidable

Details: [docs/roadmap/ROADMAP.md](docs/roadmap/ROADMAP.md).

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) and [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).

## License

[MIT](LICENSE) — Copyright (c) 2026 mcRPC contributors.
