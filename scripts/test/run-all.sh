#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
export ROOT
"$ROOT/scripts/build/desktop.sh"
ctest --test-dir "$ROOT/build" --output-on-failure
B="$ROOT/build"
"$B/examples/parser/mcrpc_example_parser"
"$B/examples/builder/mcrpc_example_builder"
"$B/examples/desktop/mcrpc_example_desktop"
"$B/examples/firmware/mcrpc_example_firmware"
"$B/examples/homeassistant/mcrpc_example_homeassistant"
"$B/mcrpc" version
"$B/mcrpc" parse "tracker#1 ping"
"$B/mcrpc" validate "all discover"
"$B/mcrpc" discover tracker
"$B/mcrpc" ping all
python3 "$ROOT/scripts/test/check-doc-links.py"
# Pure-Python protocol suite (same golden cases as C++)
if [[ -x "$ROOT/python/.venv/bin/pytest" ]]; then
  (cd "$ROOT/python" && .venv/bin/pytest -q)
elif command -v pytest >/dev/null 2>&1; then
  (cd "$ROOT/python" && PYTHONPATH=. pytest -q)
else
  echo "WARN: pytest not available — skip Python suite"
fi
echo "All tests OK"
