# Implementation plan — RFC-0001 (mcRPC 1.1)

| Field | Value |
|-------|--------|
| **RFC** | [RFC-0001 rev 03](RFC-0001-mcrpc-1.1.md) (feature-complete, architecture frozen) |
| **Rule** | Implement **only** what the RFC describes. No new protocol features. |
| **Status** | Planning document — not a release |

---

## Release train

```text
RFC Accepted (rev 03)
        │
        ▼
mcrpc 1.1.0-beta          ← Phase 1 DoD  ([DEFINITION_OF_DONE.md](../DEFINITION_OF_DONE.md))
        │
        ▼
MeshCore beta             ← Phase 2 DoD
        │
        ▼
meshcore-ha beta          ← Phase 3 DoD
        │
        ▼
Android beta              ← Phase 4 DoD
        │
        ▼
QA
        │
        ▼
Release Candidate
        │
        ▼
GA
```

Do not advance until the previous phase Definition of Done is complete.

---

## Global constraints

- Address space on RF: **name | `@id` | `all`** (+ legacy `self` / `group:`).
- No role/tag/capability **matching** in the parser or firmware dispatcher.
- Request id stays **`name#digits`**.
- Discovery becomes source of truth: full `id=`, canonical `caps=` / `features=`,
  `uptime=`, prefer `tag=` over promoting `profile=`.
- Protocol 1.0 peers remain usable where practical (`COMPATIBILITY.md`).

Open questions in the RFC (§21) are **implementation defaults**, not blockers
for starting Phase 1 (choose a default and document it in the PR).

Suggested defaults if unspecified:

| Open question | Default for first PR train |
|---------------|----------------------------|
| `tag=` + `profile=` | Emit **both** for one library minor; docs promote `tag=` only |
| `@` prefix length | Client: shortest unique ≥4 hex; device: match full id or unique prefix |
| `caps=` truncation | `caps_truncated=1` + omit overflow tokens; full list via `caps` |
| Aliases | Client-side only in Phase 3; no on-device aliases in 1.1.0 |

---

## Phase 1 — mcrpc (library / protocol core)

### Scope

- Parser: accept `@` + hex as identity target (`AddressKind` or equivalent);
  **reject** treating `#hex` as node id (digits after `#` remain request-id).
- Builders / discover helpers: full `id=`, `tag=`, `uptime=`, `protocol_min` /
  `protocol_max`, `features=`, `caps=` with §5.5 canonical CSV.
- Keep emitting legacy `profile=` when a primary tag is set (transition default).
- Document SPEC additive 1.1 section aligned with RFC (no redesign).
- Bump advertised protocol/sdk only when emitters actually speak 1.1 fields
  (coordinate with release process — may stay `1.0` advertise until cutover PR).

### Out of scope

- Role addressing, trailing request-id, response-envelope default, Service axis,
  on-device aliases.

### Estimated effort

**3–5 engineer-days** (parser + builders + tests + SPEC text).

### Dependencies

- None (starts the train). RFC rev 03 accepted as SoT.

### Expected pull requests

| PR | Content |
|----|---------|
| `mcrpc#P1a` | Parser `@id` + unit/compliance |
| `mcrpc#P1b` | Discovery/status builders + CSV canonicalization helpers |
| `mcrpc#P1c` | SPEC 1.1 additive text + COMPATIBILITY/COMMAND_COVERAGE updates |

### Release impact

- Library minor (e.g. sdk/library **1.1.0**) when behaviour ships.
- Protocol field `protocol=1.1` only after discover emitters are ready.
- No forced flash of field devices in this phase alone.

### Required golden tests

- Discover line with full `id=`, sorted `caps=` / `features=`, `uptime=`, `tag=`.
- Request `node1#7 ping` unchanged vs 1.0 golden.
- Request `@3CBBF74E ping` / `@3CBB ping` parse OK.
- `#A31C ping` does **not** become node-id (malformed or request-id path as spec’d).
- Legacy discover with only `profile=` still parses.

### Required compliance tests

- Core commands still pass on 1.0-shaped fixtures.
- Unknown discover keys ignored.
- `err unknown_command` / `unsupported` unchanged.
- CSV receiver accepts non-canonical input; emitter produces canonical.

---

## Phase 2 — MeshCore (firmware consumer)

### Scope

- Wire discovery/status through mcRPC 1.1 builders (full id from node identity,
  caps summary, uptime, tag/profile transition).
- Dispatcher: answer `Named` by **node name**; answer `@id` by full id / unique
  prefix; **no** tag-based match.
- Advertise `features=` consistent with what the build actually supports.
- Docs: `doc/` pointers to RFC; example sessions use generic names.

### Out of scope

- Changing MeshCore GRP_TXT framing, PSK model, or Chat sender prefix.
- Role dispatch tables.

### Estimated effort

**4–7 engineer-days** (host tests + 1–2 board envs + docs).

### Dependencies

- Phase 1 library tag/commit consumable via `mcrpc` dependency.

### Expected pull requests

| PR | Content |
|----|---------|
| `MeshCore#P2a` | Integrate mcrpc 1.1 builders; discovery fields |
| `MeshCore#P2b` | `@id` address match + host/regression tests |
| `MeshCore#P2c` | Doc examples / BOARDS notes (generic names only) |

### Release impact

- Firmware image bump for mcRPC-enabled envs.
- Field devices need flash to **emit** 1.1 discovery / accept `@id`; 1.0 name
  addressing continues without flash.

### Required golden tests

- Prefer shared mcrpc goldens; firmware host test: discover string shape.
- Ping by name and by `@id` on host harness if available.

### Required compliance tests

- Existing `test/mcrpc` (or equivalent) still green.
- Device named differently from `tag=` does **not** answer `ha ping`.

---

## Phase 3 — meshcore-ha (Home Assistant client)

### Scope

- TX path: **identity name or `@id` or `all` only** — never tag/capability as
  target (completions + service validation).
- Discovery cache: model from discovery SoT (id, caps, metadata, tag/profile).
- UI: optional filter/group by `tag=` / legacy `profile=` / local labels; picker
  always selects an identity before RF TX.
- Parse `@id` replies/correlation unchanged for glued request ids.
- Docs/examples: generic node names; align with RFC (no production names).

### Out of scope

- Default response-envelope on Chat RF.
- Renaming user Config Entries or production nodes.
- Implementing on-device aliases.

### Estimated effort

**5–8 engineer-days** (bridge + registry/cache + UI/docs + tests).

### Dependencies

- Phase 1 Python package behaviour (or mirrored parsers) available to HA.
- Phase 2 optional for end-to-end RF lab; unit tests can fake discovery lines.

### Expected pull requests

| PR | Content |
|----|---------|
| `meshcore-ha#P3a` | Discovery cache + parse 1.1 fields |
| `meshcore-ha#P3b` | TX validation / completions (identity only) |
| `meshcore-ha#P3c` | UI tag filter → identity send |
| `meshcore-ha#P3d` | Docs + example automations (generic) |

### Release impact

- HA integration version bump (HACS); **must** keep talking to 1.0 firmware
  (`COMPATIBILITY.md`).
- No mandatory device rename.

### Required golden / unit tests

- Build request → `node1#n cmd`, never `ha cmd` from tag filter helper.
- Cache merge from discover lines (canonical + messy CSV).
- `@` prefix resolution against cache (unique vs ambiguous).

### Required compliance / integration tests

- Existing Chat E2E / mcrpc policy / migration tests remain green.
- Regression: 1.0 discover (`profile=` only) still populates UI tag fallback.

---

## Phase 4 — Android (MeshCore Chat / companion UX)

### Scope

- Completions: identities from discovery + `all` (+ `@` short forms from cache).
- No chip/suggestion that sends tag or capability as target.
- Keep glued `name#id` rendering behaviour (existing request-id docs).
- Optional: show caps/tag as metadata in contact sheet.

### Out of scope

- Firmware changes; HA changes; new wire grammar.

### Estimated effort

**3–6 engineer-days** (depending on app module ownership).

### Dependencies

- Phase 1 discover field semantics (documentation + sample lines).
- Phase 2/3 helpful for lab but not required for offline UI tests.

### Expected pull requests

| PR | Content |
|----|---------|
| `Android#P4a` | Completions / send path identity-only |
| `Android#P4b` | Contact metadata from discover (caps, tag, id) |

### Release impact

- App store / companion release; backward compatible with 1.0 nodes.

### Required tests

- UI instrumentation or unit: selecting tag filter still sends node name.
- Request-id string still `name#digits` on the wire.

### Compliance

- Manual protocol checklist against RFC scenarios 16.6; no new mcRPC C++ suite
  required in-app if lines are built from shared rules.

---

## Cross-phase test matrix

| Case | P1 | P2 | P3 | P4 |
|------|----|----|----|-----|
| `node1#1 ping` / `#1 pong` | ✓ | ✓ | ✓ | ✓ |
| `@FULLID ping` | ✓ | ✓ | ✓ | ✓ |
| `@PREFIX` unique | ✓ | ✓ | ✓ | ✓ |
| `all discovery` 1.1 shape | ✓ | ✓ | ✓ | — |
| Canonical CSV emit | ✓ | ✓ | — | — |
| Messy CSV accept | ✓ | — | ✓ | — |
| No `ha` / `gps` as target in builders | — | ✓ | ✓ | ✓ |
| Legacy `profile=` read | ✓ | ✓ | ✓ | ✓ |

---

## Suggested sequence

```text
Phase 1 (mcrpc) ──► Phase 2 (MeshCore)
                 └─► Phase 3 (meshcore-ha)  [can overlap late Phase 1]
                        └─► Phase 4 (Android)
```

Do not release HA defaults that emit `@id` or omit 1.0-safe forms until Phase 1
parsers and Phase 2 (or lab) confirm peer behaviour.

---

## Explicitly deferred (not in this plan)

- Trailing request-id syntax
- `#hex` node ids
- Role addressing
- Response envelope as Chat default
- Service axis
- Glob / multi-target
- On-device `aliases=` (unless a later RFC)

---

*Plan tracks RFC-0001 rev 03 only.*
