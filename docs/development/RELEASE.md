# Release process

Day-to-day publish gate: **[docs/RELEASE_CHECKLIST.md](../RELEASE_CHECKLIST.md)**.
Permanent rules: **[docs/DEVELOPMENT_RULES.md](../DEVELOPMENT_RULES.md)**.

**`./scripts/release-check` is mandatory** before tag, GitHub Release, or publish.

## Steps

1. Confirm protocol, SDK, and library versions in `Version.h`, `library.json`, and `CMakeLists.txt`
2. Update `CHANGELOG.md`
3. Run `./scripts/release-check` (runs `./scripts/test/run-all.sh` + version greps)
4. Verify MeshCore consumer builds if the adapter changed
5. Tag `vMAJOR.MINOR.PATCH` (first public tag: **`v1.0.0`**)
6. Create a GitHub Release from the tag (Latest, not Draft, not Pre-release)
7. Optionally publish to the PlatformIO registry

Stress / RF envelope: meshcore-ha `docs/STRESS_METHODOLOGY.md` — never treat a
100-ping burst as a pass/fail gate.

## Version bump rules

| Change | Protocol | SDK | Library |
|--------|----------|-----|---------|
| Bugfix, no API/wire change | — | — | patch |
| Additive API | — | minor | minor |
| Additive wire field | minor | minor | minor |
| Breaking API | — | major | major |
| Breaking wire | major | major | major |
