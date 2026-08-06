# Implementation Report — RFC-0001 (mcRPC 1.1)

| Field | Value |
|-------|--------|
| RFC | RFC-0001 rev **03** (Accepted, architecture frozen) |
| Report date | 2026-08-06 |
| Train | **beta.2** |

## Beta train tips

| Component | Tag | Notes |
|-----------|-----|--------|
| mcrpc | `v1.1.0-beta.2` | Protocol/SDK 1.1; library 1.1.0-beta.2 |
| MeshCore | `mcrpc-1.1.0-beta.2` | Pins mcrpc `#v1.1.0-beta.2` |
| meshcore-ha | `v2.11.0-beta.2` | HACS tip; does not move `v2.10.1` stable |
| Android | — | Phase 4 **specification only** (`ANDROID_PHASE4.md`) |

Previous QA tips (immutable, not moved): `v1.1.0-beta`, `mcrpc-1.1.0-beta`, `v2.11.0-beta`.

## Identity ID source (meshcore-ha)

HA peer `id=` / `@id` matching uses, in order:

1. Config Entry / `coordinator.pubkey` — MeshCore companion **public key** (hex)
2. Live `api._last_self_info["public_key"]`
3. Deterministic `sha256("meshcore-ha:{entry_id}")` fallback

Documented in diagnostics as `local_peer` / `id_source`. Survives reload when pubkey is stored on the Config Entry.

## Uptime (meshcore-ha)

`uptime=` = whole seconds since this bridge instance enabled node-requests
(`time.monotonic()` from `async_setup`). Resets on integration reload/restart;
never negative.

## Compliance matrix

| Requirement | mcrpc | MeshCore | meshcore-ha | Android | Tests | Status |
|-------------|-------|----------|-------------|---------|-------|--------|
| `@id` addressing | IMPLEMENTED | IMPLEMENTED | IMPLEMENTED | DEFERRED | IMPLEMENTED | IMPLEMENTED |
| Identity-name RF only | IMPLEMENTED | IMPLEMENTED | IMPLEMENTED | DEFERRED | IMPLEMENTED | IMPLEMENTED |
| No role/tag/cap RF aliases | N/A (parser) | N/A | IMPLEMENTED | DEFERRED | IMPLEMENTED | IMPLEMENTED |
| Discovery 1.1 fields | IMPLEMENTED | IMPLEMENTED | IMPLEMENTED | DEFERRED | IMPLEMENTED | IMPLEMENTED |
| Canonical CSV caps/features | IMPLEMENTED | IMPLEMENTED | IMPLEMENTED | DEFERRED | IMPLEMENTED | IMPLEMENTED |
| `tag=` metadata only | IMPLEMENTED | IMPLEMENTED | IMPLEMENTED | DEFERRED | IMPLEMENTED | IMPLEMENTED |
| Legacy `profile=` emit/parse | IMPLEMENTED | IMPLEMENTED | IMPLEMENTED | DEFERRED | IMPLEMENTED | IMPLEMENTED |
| Request-id `#digits` | IMPLEMENTED | IMPLEMENTED | IMPLEMENTED | DEFERRED | IMPLEMENTED | IMPLEMENTED |
| Protocol 1.0 compat (named/all/glued) | IMPLEMENTED | IMPLEMENTED | IMPLEMENTED | DEFERRED | IMPLEMENTED | IMPLEMENTED |
| Caps = real capabilities | IMPLEMENTED | IMPLEMENTED | IMPLEMENTED | DEFERRED | IMPLEMENTED | IMPLEMENTED |
| Features = implemented only | IMPLEMENTED | IMPLEMENTED | IMPLEMENTED | DEFERRED | IMPLEMENTED | IMPLEMENTED |
| NodeRegistry 1.1 cache | N/A | N/A | IMPLEMENTED | DEFERRED | IMPLEMENTED | IMPLEMENTED |
| Diagnostics (no secrets) | N/A | N/A | IMPLEMENTED | DEFERRED | PARTIAL | IMPLEMENTED |
| Android client | N/A | N/A | N/A | NOT IMPLEMENTED | DEFERRED | DEFERRED |

## QA blockers fixed in beta.2

1. HA answered role aliases (`ha` / `homeassistant` / …) — **removed**; identity names + `@id` + `all` only.
2. HA ignored `AddressKind.Id` — **fixed** in `mcrpc_policy`.
3. HA discovery still 1.0-shaped — **replaced** with RFC 1.1 `<name> id=… protocol=1.1 …`.
4. Missing discovery fields — **emitted** (`id`, `tag`, `uptime`, `features`, `protocol_min/max`, …).
5. RFC header Draft / Implements Nothing — **editorial** → Accepted + beta train.
6. Heltec/LW010 — clean rebuild against beta.2 pin; **ON-AIR TEST REQUIRED BY QA** if RF not exercised here.

## Remaining debt

- Android Phase 4 implementation (spec only).
- Physical RF smoke on Heltec button + LW010 GPS hardware.
- Optional: advertise `caps-in-discovery` if HA later exposes real device caps.
