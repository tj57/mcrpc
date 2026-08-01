# Python bindings (design only — not implemented in 1.0)

## Recommended approach

1. Build `libmcrpc` (+ future `libmcrpc_c`) via CMake.
2. Expose a thin module `mcrpc` using `ctypes` or `cffi` against the C ABI.
3. Pure-Python reimplementation is discouraged unless it runs the same golden tests.

## Proposed surface

```python
class Request: ...
def parse(line: str) -> Request: ...
def strip_sender(text: str) -> str: ...
def build_request(target: str, command: str, request_id: int | None = None, args: list[str] | None = None) -> str: ...
def build_event(name: str, kv: str | None = None) -> str: ...
PROTOCOL_VERSION: str
SDK_VERSION: str
```

## Home Assistant

HA custom component should call these helpers for channel text — see `doc/HA_INTEGRATION.md` in the MeshCore tree.

## Status

**Design only.**
