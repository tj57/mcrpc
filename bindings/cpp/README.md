# C++ bindings

The C++ API **is** the native API: `#include <mcrpc/mcrpc.h>`.

This folder is reserved for:

- Optional header-only helpers (e.g. `std::string` wrappers) that must remain ABI-stable wrappers over the C API once it exists
- Example snippets for desktop tools

Do not put MeshCore-specific code here.
