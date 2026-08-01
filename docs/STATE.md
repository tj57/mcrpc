# Project state

**Version:** library 1.0.0 · SDK 1.0.0 · protocol 1.0  
**Updated:** 2026-08-01

## Maturity

| Area | Status |
|------|--------|
| Wire protocol | Stable |
| C++ public API | Stable (1.x additive) |
| Reference implementation | This repository |
| Compliance / golden suites | Required on CI |
| CLI | First-class |
| Language bindings | Design only |
| Home Assistant component | Docs only |

## Remaining technical debt

- Feature handlers use file-static `g_*` pointers (implementation detail)
- No C ABI yet (blocks easy Python/Kotlin FFI)
- No `mcrpc_core` CMake target without reference features
- PlatformIO firmware CI is maintained out-of-tree (MeshCore Docker builds)
- `LedFeature` is header-only stub (not linked into default library)

## Suitability

Ready for public GitHub publication as **v1.0.0** once the remote exists and the release checklist is signed off.
