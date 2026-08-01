# Home Assistant integration

Use the **pure-Python** package in [`python/`](../python/) — do **not** reimplement
the grammar inside meshcore-ha.

## Parse inbound

1. Receive channel text from the MeshCore transport.
2. `strip_sender_prefix`.
3. Branch:
   - `event …` → fire HA event / trigger
   - `#id …` → correlate with outstanding request
   - discover/status data → structured parameters
   - otherwise classify with `parse_response`

## Build commands

```python
from mcrpc import build_request
build_request("tracker", "gps", request_id=42)
# → "tracker#42 gps"
```

## Events

`event <name> [k=v …]` — map name to automations / future entity bridge.

## Request IDs

Use `RequestCorrelator` (monotonic IDs + timeout).

## Compliance reuse

```bash
cd python && pytest
```

Golden cases live in `tests/golden/cases` (shared with C++).

See MeshCore HA fork docs: `docs/MCRPC.md` (meshcore-ha `mcrpc` branch).
