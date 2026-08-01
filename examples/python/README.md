# Future Python binding example (design)

```python
# Not implemented in 1.0 — see bindings/python/README.md
import mcrpc

req = mcrpc.parse("tracker#42 gps")
assert req.command == "gps"
assert req.request_id == 42

line = mcrpc.build_request("tracker", "gps", request_id=42)
assert line == "tracker#42 gps"
```

Run C++ golden tests until the Python binding exists; then re-run the same
`tests/golden/cases/*.txt` files through Python.
