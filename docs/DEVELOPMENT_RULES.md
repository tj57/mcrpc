# Development rules (mandatory)

These rules apply to the mcRPC library and its MeshCore / Home Assistant consumers.

## Git history

- Author must always be **`tj57 <tj57@users.noreply.github.com>`**.
- Never add **`Co-authored-by: Cursor`**.
- Never rewrite the **entire** history of a consumer fork.
- If commit cleanup is required, rewrite **only**
  `upstream/main..feature_branch` or use `format-patch` + `git am`.
- Never run **`git filter-repo`** / **`git filter-branch`** against complete
  upstream history.

Details: [development/COMMIT.md](development/COMMIT.md).

## Release

Every release requires **`./scripts/release-check` PASS** (wraps
`scripts/release/check.sh`) before tag, GitHub Release, or publication.

## Protocol

- Public channel: never for positive tests; one negative test only.
- Positive tests: **mcCtrl**.
- `err unknown_command` = no handler; `err unsupported` = handler present,
  feature unavailable (SPEC §18).
