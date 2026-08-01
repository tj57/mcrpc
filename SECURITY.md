# Security Policy

## Supported versions

| Version | Supported |
|---------|-----------|
| 1.0.x   | Yes |

## Reporting a vulnerability

Please use GitHub **Security Advisories** on this repository when available,
or contact the maintainers privately.

Include:

- Output of `mcrpc version`
- Whether the issue is in the parser, dispatcher, or a feature
- A minimal text input (golden-test form preferred)

## Scope

mcRPC is an application-layer text protocol library.

Transport encryption, authentication, and radio security are **out of scope**
and belong to the transport adapter (for example MeshCore channel crypto).
