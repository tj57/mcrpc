# Migrating MeshCore to standalone mcRPC

## Current workspace (recommended)

```
/data/projects/
  mcrpc/                 # only protocol implementation
  meshcore/              # consumer; examples/mcrpc adapter only
```

PlatformIO (committed):

```ini
[platformio]
lib_extra_dirs = ..

; in mcRPC firmware envs:
lib_deps =
  …
  mcrpc
```

No `meshcore/lib/mcrpc` sources. No symlinks. Library is used **in place**.

## Alternatives

### `file://../mcrpc`

Copies into `.pio/libdeps` — works, but stale until reinstall. Prefer `lib_extra_dirs`.

### Git submodule

```bash
cd meshcore
git submodule add <mcrpc-remote-url> lib/mcrpc
```

### Symlink (discouraged)

```bash
ln -sfn ../mcrpc lib/mcrpc
```

## See also

- `/data/projects/README.md`
- `doc/WORKSPACE.md` (MeshCore)
