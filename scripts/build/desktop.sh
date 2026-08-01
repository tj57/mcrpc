#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD="${ROOT}/build"
cmake -S "$ROOT" -B "$BUILD" \
  -DMCRPC_BUILD_TESTS=ON \
  -DMCRPC_BUILD_CLI=ON \
  -DMCRPC_BUILD_EXAMPLES=ON
cmake --build "$BUILD" -j"$(nproc 2>/dev/null || echo 2)"
echo "Build OK: $BUILD"
