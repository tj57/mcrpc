# mcRPC Python

Pure-Python implementation of the **mcRPC** protocol, matching the C++ reference
in this repository (`include/mcrpc`, `src/`).

Home Assistant and other Python peers should depend on this package — **do not**
reimplement the grammar inside consumer projects.

## Install (development)

```bash
cd /data/projects/mcrpc/python
pip install -e ".[dev]"
pytest
```

Golden and compliance suites reuse `../tests/golden/cases` and mirror
`../tests/compliance`.

## Public API

```python
from mcrpc import (
    parse,
    strip_sender_prefix,
    build_request,
    build_event,
    parse_response,
    parse_status,
    parse_discover,
    parse_gps,
    parse_battery,
    PROTOCOL_VERSION,
    SDK_VERSION,
)
```

See `docs/protocol/SPEC.md` and `docs/homeassistant.md` in the repo root.
