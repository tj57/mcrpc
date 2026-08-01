# Release checklist — mcRPC 1.0.0

Use this before tagging `v1.0.0` or opening the GitHub repository to the public.

## Repository

- [x] Canonical tree at `/data/mcrpc` (no nested MeshCore sources)
- [x] No `build/` artifacts committed
- [x] No accidental symlinks inside the repository
- [x] `.gitignore` excludes build outputs
- [x] Layout documented in `docs/architecture/LAYOUT.md`

## README & docs

- [x] README explains what / why / quick start / build / examples / docs / license / status
- [x] No duplicate conflicting roadmap files (root `ROADMAP.md` points to `docs/roadmap/`)
- [x] Protocol SPEC, API, Architecture, Transport, Testing, Migration present
- [x] Internal links verified via `scripts/test/check-doc-links.py`
- [x] `docs/STATE.md` and this checklist present

## Tests

- [x] `./scripts/test/run-all.sh` passes (unit, compliance, golden, integration, examples, CLI, doc links)
- [x] MeshCore consumer builds (`Heltec_v3_mcrpc_button`, `LW010_mcrpc_gps`) when adapter present

## Version

- [x] `Version.h`: protocol `1.0`, SDK `1.0.0`, library `1.0.0`
- [x] `CMakeLists.txt` project `VERSION 1.0.0`
- [x] `library.json` `"version": "1.0.0"`
- [x] CLI `mcrpc version` reports all three
- [x] Recommended git tag: **`v1.0.0`** (create after first commit; do not push from this prep)

## License

- [x] `LICENSE` (MIT)
- [x] `license.txt` (same text, tooling-friendly name)
- [x] README license section
- [x] `library.json` `"license": "MIT"`

## GitHub readiness

- [x] `.github/workflows/ci.yml`
- [x] Issue templates (bug / feature)
- [x] Pull request template
- [x] `CODE_OF_CONDUCT.md`
- [x] `SECURITY.md`
- [x] `CONTRIBUTING.md`
- [x] `FUNDING.yml` (placeholder)
- [ ] Remote created (manual)
- [ ] Push + tag (manual — **do not automate in this prep**)

## Sign-off

| Check | Status |
|-------|--------|
| Repository ready | Yes (local) |
| README reviewed | Yes |
| Docs reviewed | Yes |
| Tests passing | Yes |
| Examples passing | Yes |
| Version verified | Yes |
| License verified | Yes |
| GitHub ready | Yes (pending remote) |

When the remote exists and the tag is pushed, the repository is ready for immediate public publication.

