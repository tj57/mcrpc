#!/usr/bin/env bash
set -euo pipefail
# Pre-release sanity — see docs/development/RELEASE.md + docs/DEVELOPMENT_RULES.md
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"

echo "== Docs (RC policy) =="
test -f "$ROOT/docs/DEVELOPMENT_RULES.md"
test -f "$ROOT/docs/protocol/COMMAND_COVERAGE.md"
grep -qi 'Public channel is' "$ROOT/docs/DEVELOPMENT_RULES.md" || grep -qi 'Public' "$ROOT/docs/DEVELOPMENT_RULES.md"
grep -q 'out of scope' "$ROOT/docs/DEVELOPMENT_RULES.md"
grep -q '100-ping' "$ROOT/docs/protocol/COMMAND_COVERAGE.md" || grep -q '100-ping' "$ROOT/docs/DEVELOPMENT_RULES.md"
grep -q 'Warm-up' "$ROOT/docs/protocol/COMMAND_COVERAGE.md"
! grep -qiE 'one negative|reserved for.*negative' "$ROOT/docs/protocol/COMMAND_COVERAGE.md"

"$ROOT/scripts/test/run-all.sh"
grep -q '1.1.0-beta' "$ROOT/include/mcrpc/Version.h"
grep -q 'MCRPC_PROTOCOL_VERSION_STRING "1.1"' "$ROOT/include/mcrpc/Version.h"
grep -q '"version": "1.1.0-beta"' "$ROOT/library.json"
test -f "$ROOT/docs/DEFINITION_OF_DONE.md"
test -f "$ROOT/docs/rfc/RFC-0001-mcrpc-1.1.md"
echo "Release checks OK / release-check PASS"
