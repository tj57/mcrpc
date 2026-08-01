# C bindings (design only — not implemented in 1.0)

Stable **C ABI** planned for SDK 1.1 so Python/Kotlin/Go can FFI without C++ name mangling.

## Proposed header `mcrpc_c.h`

```c
typedef struct mcrpc_request mcrpc_request;
typedef struct mcrpc_buffer mcrpc_buffer; /* fixed char buffer */

int mcrpc_protocol_version(char* out, size_t n);
int mcrpc_sdk_version(char* out, size_t n);

int mcrpc_parse(const char* line, mcrpc_request* out); /* 0=ok */
int mcrpc_strip_sender(const char* in, const char** out_line);

int mcrpc_build_request(mcrpc_buffer* out, const char* target,
                        uint32_t request_id /*0=none*/, const char* command,
                        int argc, const char* const* argv);
int mcrpc_build_event(mcrpc_buffer* out, const char* name, const char* kv);
int mcrpc_build_error(mcrpc_buffer* out, const char* code);

/* Optional: opaque dispatcher for simulators — not required for HA */
```

## Rules

- Expose only parse/build/version (+ optional dispatch).
- No FeatureManager / HostServices in the C ABI.
- Opaque structs; no MeshCore types.
- Error codes as negative ints; wire `err` strings via builders.

## Status

**Stub.** Implementation deferred to 1.1.
