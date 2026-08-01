# CLI example

The first-class CLI is built from `tools/mcrpc_cli` as the `mcrpc` binary:

```bash
cmake -S . -B build && cmake --build build --target mcrpc
./build/mcrpc parse "tracker#1 ping"
./build/mcrpc ping tracker
./build/mcrpc discover tracker
./build/mcrpc version
```
