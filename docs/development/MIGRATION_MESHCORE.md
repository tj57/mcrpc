# Migrating MeshCore to standalone mcRPC

## Workspace

```
/data/projects/
  mcrpc/                 # only protocol implementation
  meshcore/              # consumer; examples/mcrpc adapter only
```

## Dependency modes

| Mode | Configuration |
|------|----------------|
| Development | `meshcore/platformio.local.ini` (gitignored) sets `[mcrpc_lib] lib = symlink://../mcrpc` |
| Release / CI | committed `[mcrpc_lib] lib = https://github.com/…/mcrpc.git#vX.Y.Z` |
| Future | `mcrpc @ ^1.0.0` on PlatformIO Registry |

Firmware envs reference `${mcrpc_lib.lib}` only — no duplicated `platformio.ini` trees.

See `/data/projects/README.md` and `meshcore/doc/MCRPC_DEPENDENCY.md`.
