# Go bindings (design only — not implemented in 1.0)

## Recommended approach

`cgo` against the future C ABI (`mcrpc_c.h`).

```go
package mcrpc

func Parse(line string) (Request, error)
func BuildRequest(target, command string, id uint32, args ...string) string
func ProtocolVersion() string
func SDKVersion() string
```

Run golden files from `tests/golden/cases` in `go test`.

## Status

**Design only.**
