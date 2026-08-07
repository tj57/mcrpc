# Development rules (mandatory)

These rules apply to the mcRPC library and its MeshCore / Home Assistant consumers.

## Git

- Author is always **`tj57 <tj57@users.noreply.github.com>`**.
- Never add **`Co-authored-by: Cursor`**.
- Never rewrite the complete history of a consumer fork.
- Cleanup only: `upstream/main..feature_branch` or `format-patch` + `git am`.
- Never run **`filter-repo`** / **`filter-branch`** on complete upstream history.

Details: [development/COMMIT.md](development/COMMIT.md).

## Configuration

Developer must never modify production Home Assistant configuration, and must
never rename the production node (**ha-peer**), production channels (**ha-peer**),
or production PSKs during development or automated tests.

Temporary changes: **backup → test → restore automatically**.

Config Entry title may remain **lab-channel** (cosmetic) while the device name is
**ha-peer** — they may differ; no rename / migration of the entry title.

## QA

Developer never generates prompts.
Developer never modifies QA prompts.
Developer only fixes code.

## Protocol

- Public channel is **out of scope** for mcRPC (MeshCore Chat only).
- Protocol testing uses the private channel / HA Config Entry path.
- `err unknown_command` vs `err unsupported` per SPEC §18.

## Release

**`./scripts/release-check` PASS** is mandatory before tag, GitHub Release, or
publication.

## Stress

Use the realistic paced methodology in meshcore-ha `docs/STRESS_METHODOLOGY.md`
— not a 100-ping RF pass/fail burst. Saturation → busy / queue_full / table_full
/ backpressure.
