# Extraction record

Date: 2026-08-01

## What moved

- Protocol library sources → `include/` + `src/`
- Host/compliance/golden/fuzz tests → `tests/`
- CLI → `tools/mcrpc_cli`
- Binding designs → `bindings/`
- Normative protocol → `docs/protocol/SPEC.md`

## What stayed in MeshCore

- `examples/mcrpc` transport adapter
- Board variants / PlatformIO envs
- Mesh-specific documentation

## API change at extract (intentional cleanup)

- Removed `Config::begin(void*)` (unused by adapter; `ConfigStore` only)
- Deleted empty `Registry` compatibility TU
- Headers use `#include <mcrpc/...>` exclusively

## Protocol behavior

Unchanged from pre-extract 1.0 library (including `stripSenderPrefix` whitespace rule).
