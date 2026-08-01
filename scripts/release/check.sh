#!/usr/bin/env bash
set -euo pipefail
# Pre-release sanity — see docs/development/RELEASE.md
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
"$ROOT/scripts/test/run-all.sh"
grep -q '1.0.0' "$ROOT/include/mcrpc/Version.h"
grep -q '"version": "1.0.0"' "$ROOT/library.json"
echo "Release checks OK"
