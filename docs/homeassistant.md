# Home Assistant integration (design)

Do **not** reimplement the grammar in Python. Link libmcrpc (via future C ABI)
or run the same golden tests against a careful port.

## Parse inbound

1. Receive channel text from the MeshCore (or other) transport.
2. `stripSenderPrefix`.
3. Branch:
   - `event …` → fire HA event / trigger
   - `#id …` → correlate with outstanding request
   - discover/status data → update device registry
   - otherwise try `Parser::parse` if acting as a gateway

## Build commands

Use `OutboundBuilder::request` / `requestWithArgs` only:

```
tracker#42 gps
all discover
```

## Events

`event <name> [k=v …]` — map name to entity platform.

## Request IDs

Monotonic per HA instance; timeout outstanding commands.

## Compliance reuse

Ship or CI-run `tests/golden/cases` against the binding.

See also `examples/homeassistant/` for a C++ peer sketch.
