# Protocol compatibility statement

**mcRPC Protocol 1.2** (`v=1.2`) is the current wire contract for new emitters
([RFC-0002](../rfc/RFC-0002-mcrpc-1.2-slim-call.md)).

The C++ code in this repository is the **reference implementation**.

Any other implementation (Python, Kotlin, Go, alternate C++) is compatible when:

1. It passes `tests/compliance/`
2. It produces matching outputs for `tests/golden/cases/`
3. Discover advertises `v=` with the same **major** version (legacy `protocol=` still readable)

## Forward compatibility (MUST)

> **Unknown fields in `discovery` responses MUST be ignored.**  
> **Unknown fields in `status` responses MUST be ignored.**

Unknown `key=value` pairs in `call` results MUST likewise be ignored.

`err unknown_command` means no handler; `err unsupported` means handler present
but feature unavailable; `err unknown_proc` means `call` with an unknown
`ns.action`.

---

## Protocol 1.2 (RFC-0002)

| Topic | Rule |
|-------|------|
| Discovery | Slim: `id` (8 hex), `fw`, `v`, optional `tag`/`up`/`caps` |
| Status | Rich: `id_full`, radio, power, heap, `transport`, … |
| `call` | Ordinary command; `ns.action`; results `ok`/`err`/`busy`/`retry` + kv only |
| Parser | `call` MUST NOT change parser state |

**Soft rule:** Implementations SHOULD maintain backward compatibility with
Protocol 1.0 / 1.1 readers where practical (ignore unknown; accept legacy
`profile=` / `protocol=` / `uptime=` when present).

### Informative expectations

| Actor | Guidance |
|-------|----------|
| **On-air 1.0/1.1 devices** | Keep working with upgraded clients that still accept legacy discover fields. |
| **New 1.2 emitters** | Do not emit `protocol*` / `sdk` / `features=` / `transport=` / `profile=` on discovery. |
| **meshcore-ha** | Prefer identity targets; map UI tags client-side; handle `call` + dotted events. |
| **CSV lists** | Emitters: lowercase, unique, alphabetically sorted; receivers still accept messy lists. |

Breaking removals of Named addressing belong in a future **major** protocol revision.

---

## Protocol 1.1 (RFC-0001) — historical

[`RFC-0001-mcrpc-1.1.md`](../rfc/RFC-0001-mcrpc-1.1.md) clarified `@id` addressing
and discovery metadata. Superseded for discovery shape by RFC-0002; addressing
rules remain in force.
