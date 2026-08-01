# Development workspace

```
/data/projects/mcrpc      ← this repository
/data/projects/meshcore   ← MeshCore consumer
```

| Mode | MeshCore `lib_deps` |
|------|---------------------|
| Development | `symlink://../mcrpc` via `platformio.local.ini` |
| Release | `https://github.com/…/mcrpc.git#vX.Y.Z` in committed `[mcrpc_lib]` |

See `/data/projects/README.md` and `/data/projects/docker/README.md`.
