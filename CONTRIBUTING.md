# Contributing to mcRPC

Thanks for helping. Protocol and SDK are **stable** — prefer additive fixes.

## Ground rules

1. Keep protocol specification and implementation in sync.
2. Do not add transport SDKs (MeshCore, MQTT, Arduino) to `include/` or `src/`.
3. Update compliance and/or golden tests when wire behaviour changes.
4. Document public API changes in `CHANGELOG.md`.

## Development

```bash
./scripts/test/run-all.sh
```

## Pull requests

- One focused change per PR
- Fill in the PR template
- Link related issues
- Do not bump major versions without discussion

## Code of conduct

Please follow [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).
