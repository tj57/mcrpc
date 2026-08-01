# Repository layout

```
mcrpc/
├── include/mcrpc/          # PUBLIC API (installed)
│   ├── mcrpc.h             # umbrella
│   ├── FeatureSdk.h
│   ├── Version.h
│   ├── … core headers …
│   └── features/           # reference feature headers
├── src/                    # PRIVATE implementation (.cpp only)
├── tests/
│   ├── unit/
│   ├── compliance/         # official compatibility suite
│   ├── golden/cases/       # protocol contract examples
│   ├── fuzz/
│   └── integration/
├── examples/
│   ├── parser/
│   ├── builder/
│   ├── desktop/
│   ├── firmware/           # HostServices stub (no Arduino)
│   ├── homeassistant/
│   ├── cli/                # docs → tools/mcrpc_cli
│   ├── meshcore/           # docs → MeshCore adapter
│   └── python/             # future binding sketch
├── bindings/               # design stubs (not implemented)
├── docs/
│   ├── architecture/
│   ├── protocol/
│   ├── api/
│   ├── development/
│   ├── roadmap/
│   ├── homeassistant.md
│   ├── STATE.md
│   └── RELEASE_CHECKLIST.md
├── tools/mcrpc_cli/
├── scripts/{build,test,release}/
├── cmake/
├── assets/
├── .github/
│   ├── workflows/
│   ├── ISSUE_TEMPLATE/
│   ├── PULL_REQUEST_TEMPLATE.md
│   └── FUNDING.yml
├── CMakeLists.txt
├── library.json
├── README.md
├── CHANGELOG.md
├── CONTRIBUTING.md
├── CODE_OF_CONDUCT.md
├── ROADMAP.md
├── SECURITY.md
├── LICENSE
└── license.txt
```

## Design rationale (protobuf / mbedTLS style)

| Choice | Why |
|--------|-----|
| `include/` vs `src/` | Only stable headers are installable; `.cpp` never ships as API |
| Features in `include/…/features/` | Reference command semantics are part of the SDK |
| No transport in-tree | Same as mbedTLS not shipping Ethernet drivers |
| Golden + compliance in-repo | Official interop suite for other languages |
| Examples compile on desktop | Prove zero embedded SDK dependency |

## What is not in this repo

- MeshCore radio stack, variants, PlatformIO board envs
- Arduino FS / GPS power drivers (`examples/mcrpc` in MeshCore)
- Full language bindings (design only)
- Home Assistant custom component
