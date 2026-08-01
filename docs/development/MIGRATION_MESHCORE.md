# Migrating MeshCore to standalone mcRPC

## Before

```
MeshCore/lib/mcrpc/     # library sources lived here
MeshCore/examples/mcrpc # adapter
```

## After

```
mcrpc/                     # this repository (standalone)
MeshCore/lib/mcrpc ->      # dependency (submodule, symlink, or PlatformIO lib)
MeshCore/examples/mcrpc    # thin adapter only (unchanged role)
```

## Recommended consumption

### Option A — git submodule

```bash
cd MeshCore
git submodule add https://github.com/mcrpc/mcrpc.git lib/mcrpc
```

### Option B — PlatformIO `lib_extra_dirs`

```ini
lib_extra_dirs = ../mcrpc
lib_deps = mcrpc
```

### Option C — local symlink (development)

```bash
ln -sfn /path/to/mcrpc MeshCore/lib/mcrpc
```

## Adapter checklist

- [x] `#include <mcrpc/...>` (include root = library `include/`)
- [x] `ConfigStore` for persistence (no `Config::begin(void*)`)
- [x] `HostServices` on the mesh class
- [x] `PublishFn` → group text send
- [x] No protocol parsing outside libmcrpc

## Breaking notes for 1.0 extract

- Public headers moved to `include/mcrpc/` (PlatformIO `library.json` updated).
- Removed `Config::begin(void*)`.
- Removed empty `Registry` TU.
- Protocol docs live in mcRPC; MeshCore keeps board/channel docs.
