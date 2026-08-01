# Protocol compatibility statement

**mcRPC Protocol 1.0** is the wire contract.

The C++ code in this repository is the **reference implementation**.

Any other implementation (Python, Kotlin, Go, alternate C++) is compatible when:

1. It passes `tests/compliance/`
2. It produces matching outputs for `tests/golden/cases/`
3. Discover advertises `protocol=` with the same **major** version

Clients MUST ignore unknown `key=value` fields in status/discover/events.

`err unknown_command` means no handler; `err unsupported` means handler present
but feature unavailable.

This statement supersedes informal notes in older MeshCore draft docs.
