# Release process

Day-to-day publish gate: **[docs/RELEASE_CHECKLIST.md](../RELEASE_CHECKLIST.md)**.

## Steps

1. Confirm protocol, SDK, and library versions in `Version.h`, `library.json`, and `CMakeLists.txt`
2. Update `CHANGELOG.md`
3. Run `./scripts/test/run-all.sh`
4. Verify MeshCore consumer builds if the adapter changed
5. Tag `vMAJOR.MINOR.PATCH` (first public tag: **`v1.0.0`**)
6. Create a GitHub Release from the tag (manual)
7. Optionally publish to the PlatformIO registry

## Version bump rules

| Change | Protocol | SDK | Library |
|--------|----------|-----|---------|
| Bugfix, no API/wire change | — | — | patch |
| Additive API | — | minor | minor |
| Additive wire field | minor | minor | minor |
| Breaking API | — | major | major |
| Breaking wire | major | major | major |
