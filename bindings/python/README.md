# Python bindings

**Implemented** as a pure-Python package under [`../../python/`](../../python/) that
passes the same golden and compliance suites as the C++ reference.

```bash
cd python
python3 -m venv .venv && .venv/bin/pip install -e ".[dev]"
.venv/bin/pytest
```

Home Assistant should depend on this package — do not reimplement the grammar
inside meshcore-ha.

A future optional path may wrap `libmcrpc` via cffi; the public surface should
remain the same.
