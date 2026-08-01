# Migrating MeshCore to standalone mcRPC

## Current workspace (recommended)

```
/data/projects/
  mcrpc/                 # only protocol implementation
  meshcore/              # consumer; examples/mcrpc adapter only
```

PlatformIO (committed in mcRPC firmware envs):

```ini
lib_deps =
  …
  file://../mcrpc
```

No `meshcore/lib/mcrpc` sources. No symlinks.

## Alternatives

### Git submodule

```bash
cd meshcore
git submodule add <mcrpc-remote-url> lib/mcrpc
# then lib_deps = mcrpc   (LDF finds lib/mcrpc)
```

### `lib_extra_dirs`

```ini
[platformio]
lib_extra_dirs = ..
# with lib_deps = mcrpc  → finds ../mcrpc/library.json
```

### Symlink (discouraged)

Only if PlatformIO path refs are unavailable:

```bash
ln -sfn ../mcrpc lib/mcrpc
```

## Adapter checklist

- [x] `#include <mcrpc/...>`
- [x] `ConfigStore` for persistence
- [x] `HostServices` on the mesh class
- [x] `PublishFn` → group text send
- [x] No protocol parsing outside `/data/projects/mcrpc`

## See also

- `/data/projects/README.md`
- `doc/WORKSPACE.md` (in MeshCore)
