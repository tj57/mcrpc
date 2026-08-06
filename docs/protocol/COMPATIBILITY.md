# Protocol compatibility statement

**mcRPC Protocol 1.0** is the current wire contract on the air.

The C++ code in this repository is the **reference implementation**.

Any other implementation (Python, Kotlin, Go, alternate C++) is compatible when:

1. It passes `tests/compliance/`
2. It produces matching outputs for `tests/golden/cases/`
3. Discover advertises `protocol=` with the same **major** version

Clients MUST ignore unknown `key=value` fields in status/discover/events.

`err unknown_command` means no handler; `err unsupported` means handler present
but feature unavailable.

---

## Protocol 1.1 (draft — see RFC-0001)

[`RFC-0001-mcrpc-1.1.md`](../rfc/RFC-0001-mcrpc-1.1.md) proposes an additive
clarification of addressing and discovery metadata.

**Soft rule:** Implementations SHOULD maintain backward compatibility with
Protocol 1.0 where practical.

### Informative expectations (not product mandates inside the RFC abstract)

| Actor | Guidance |
|-------|----------|
| **On-air 1.0 devices** | Keep working with upgraded clients without a mandatory flash when clients still emit 1.0-safe request forms (`name` / `all`, glued `name#id`). |
| **meshcore-ha** | Prefer identity targets on RF; map any UI “role” filters to concrete names/`@id` before TX. Accept legacy replies and discovery fields. |
| **New 1.1 fields** | Full `id=`, canonical `caps=` / `features=`, `uptime=`, `tag=`, `@id` targets — additive; 1.0 peers ignore unknown keys. |
| **Legacy `profile=`** | Informational only; prefer `tag=` in new emitters. Not an address kind. |
| **CSV lists** | 1.1 emitters: lowercase, unique, alphabetically sorted; receivers still accept messy legacy lists. |

Breaking removals (e.g. refusing Named targets that look like old profile labels)
belong in a future **major** protocol revision, not 1.1.

This statement supersedes informal notes in older MeshCore draft docs.
