# Android Phase 4 — RFC-0001 (mcRPC 1.1)

**Status:** No Android companion tree is present in `/data/projects`.
This document is the **Phase 4 implementation contract** for when that repo
is available.

## DoD (from `docs/DEFINITION_OF_DONE.md`)

- [ ] Completions: discovery **names** + `all` + `@` short forms from cache
- [ ] No tag / capability send chips (`ha`, `gps`, `button`, …)
- [ ] Request-id remains `name#digits`
- [ ] Contact sheet shows `id`, `tag`, `caps`, `fw`, `board` from discovery
- [ ] Works with Protocol 1.0 and 1.1 discovery lines
- [ ] App CI / instrumentation green

## Wire rules (must match RFC-0001)

| Allowed TX | Forbidden TX |
|------------|--------------|
| `node1 ping` | `ha ping` |
| `@3CBB status` | `gps ping` |
| `all discovery` | `button ping` |

UI may **filter** by `tag=ha` then send the selected **identity**.

## Suggested PR split

1. Completions + send path identity-only
2. Contact metadata from discover SoT fields

## Dependency

Consume discovery lines produced by MeshCore **beta** / mcrpc **1.1.0-beta**.
