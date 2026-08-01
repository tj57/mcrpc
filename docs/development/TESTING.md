# Testing

## Suites

| Suite | Path | Role |
|-------|------|------|
| Unit | `tests/unit/` | Framework internals |
| Compliance | `tests/compliance/` | Official protocol contract |
| Golden | `tests/golden/cases/` | IN/OUT examples |
| Integration | `tests/integration/` | Fake transport e2e |
| Fuzz | `tests/fuzz/` | Optional libFuzzer |

```bash
./scripts/test/run-all.sh
```

Other language implementations SHOULD reuse golden cases and compliance categories.

## Fuzz

```bash
cmake -S . -B build-fuzz -DMCRPC_BUILD_FUZZERS=ON -DCMAKE_CXX_COMPILER=clang++
cmake --build build-fuzz --target mcrpc_fuzz_parser
```
