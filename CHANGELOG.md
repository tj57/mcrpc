# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/).
This project uses independent **protocol**, **SDK**, and **library** versions
(see `include/mcrpc/Version.h`).

## [1.2.4] — 2026-08-07

### SDK / Library

- SDK **1.2.4**, library **1.2.4**
- `RequestCorrelator`: known request lines (`call … entity=`, `all ping`, …) classify as ``other``, not ``Data``
- Fixes peer→HA inbound ``call`` with kv args never producing ``#N ok`` / bus event

## [1.2.3] — 2026-08-07

Wider broadcast stagger so half-duplex peers stop sharing RF slots.

### Protocol

- Recommended `ReplyJitter` window: **400–3600 ms**, **16 slots**
- Slot seed SHOULD be discovery `id=` (first 8 hex) when identity is pubkey hex

### SDK / Library

- SDK **1.2.3**, library **1.2.3**
- Python `delay_seconds(..., companion_bias=, local_tx_settle=)` for companion hosts
- Wire `v=` remains **1.2**

## [1.2.2] — 2026-08-07

RFC-0002 §8: broadcast reply stagger + library `ReplyJitter` / PublishEx.

### Protocol

- Emitters answering `all` MUST delay TX (250–1750 ms, per-node slot + entropy)
- Events and addressed replies stay outside the broadcast window

### SDK / Library

- SDK **1.2.2**, library **1.2.2**
- `ReplyJitter.h`, Python `reply_jitter`
- `McRpc::setPublishExHandler` / `publishRaw(text, delay_ms)`
- Wire `v=` remains **1.2**

## [1.2.1] — 2026-08-07

Patch: expose Python `format_uptime` / `short_id8` (parity with C++ `UptimeFormat` / short id).

### SDK / Library

- SDK **1.2.1**, library **1.2.1**
- Protocol wire remains **1.2**

## [1.2.0] — 2026-08-07

RFC-0002 (protocol **1.2**): slim discovery, rich status, namespaced `call`.

### Protocol

- Discovery emits `id` (8 hex), `fw`, `v=1.2`, optional `tag` / `up` / `caps` only
- Dropped from discovery emitters: `protocol*`, `sdk`, `features=`, `transport=`, `profile=`, full-length id
- Status carries `id_full`, `transport`, radio/power/heap contributions
- Human `up=` uptime (`2h33m`); unknown discover/status fields MUST be ignored
- Core command `call ns.action` (parser-neutral); results `ok`/`err`/`busy`/`retry` + `key=value` only
- Events: dotted names (`button.pressed`, `battery.low`)

### SDK / Library

- SDK **1.2.0**, library **1.2.0**
- `CallResult`, `UptimeFormat` / `shortId8`
- `HostServices::handleCall`, `readSnr`, `readHeapFree`

## [1.1.0] — 2026-08-06

RFC-0001 GA (protocol **1.1**). Architecture frozen at rev 03.

### Highlights

- Identity addressing: name / `@id` / `all` (no role/tag RF addresses)
- Discovery 1.1: `id=`, `tag=`, `uptime=`, `protocol_min`/`protocol_max`, canonical `caps=`/`features=`
- Library **1.1.0**, SDK **1.1.0**, protocol **1.1**

## [1.1.0-beta.2] — 2026-08-06

RFC-0001 QA fix train (editorial RFC status; discover `features=` CSV tokens).

### Changed

- Library **1.1.0-beta.2**
- Python discover parser exposes `feature_tokens` / `protocol_min` / `protocol_max`
- RFC-0001 header: Accepted rev 03 (implementation beta train)

## [1.1.0-beta] — 2026-08-06

RFC-0001 Phase 1 beta (architecture frozen — additive only).

### Protocol (1.1)

- Identity addressing via `@` + hex (`AddressKind::Id`); `#` + digits remains request-id only
- Discovery metadata: full `id=`, `tag=`, `uptime=`, `protocol_min` / `protocol_max`, `features=`, `caps=` CSV
- Canonical CSV rules for `caps=` / `features=` (lowercase, unique, sorted)
- Legacy `profile=` still emitted for compatibility; prefer `tag=`

### SDK / Library

- SDK **1.1.0**, library **1.1.0-beta**
- `Dispatcher::setNodeId`, `McRpc::setNodeId` / `setTag`
- `CanonicalCsv.h` helpers

### Docs

- RFC-0001 rev 03, implementation plan, Definition of Done, release train

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
