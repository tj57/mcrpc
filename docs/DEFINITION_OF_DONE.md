# Definition of Done — RFC-0001 implementation

Source of truth: [`RFC-0001-mcrpc-1.1.md`](rfc/RFC-0001-mcrpc-1.1.md),
[`IMPLEMENTATION_PLAN-RFC-0001.md`](rfc/IMPLEMENTATION_PLAN-RFC-0001.md).

Architecture is frozen. A phase is **DONE** only when every checkbox for that
phase is satisfied.

---

## Release train

```text
RFC Accepted (rev 03)
        │
        ▼
mcrpc 1.1.0-beta          ← Phase 1 DoD
        │
        ▼
MeshCore beta             ← Phase 2 DoD
        │
        ▼
meshcore-ha beta          ← Phase 3 DoD
        │
        ▼
Android beta              ← Phase 4 DoD
        │
        ▼
QA (implementation / RF lab)
        │
        ▼
Release Candidate
        │
        ▼
GA
```

Rules:

- Do not advance the train until the previous phase DoD is complete.
- Beta tags may advertise `protocol=1.1` only when emitters actually speak 1.1
  fields.
- GA requires RC + QA sign-off; no architecture changes on the train.

---

## Phase 1 DONE — mcrpc

- [ ] Parser accepts `@` + hex identity targets
- [ ] `#` + digits remains request-id only (`#A31C` is not a node id)
- [ ] Named / `all` / `self` / `group:` behaviour unchanged for 1.0 forms
- [ ] Discover/status helpers: full `id=`, `tag=`, `uptime=`, `protocol_min` /
      `protocol_max`, canonical `caps=` / `features=`
- [ ] Legacy `profile=` still readable; new emitters prefer `tag=` (may emit both)
- [ ] Unit tests OK
- [ ] Compliance tests OK
- [ ] Golden tests OK (1.0 cases still pass; 1.1 cases added)
- [ ] Fuzz / existing fuzz job OK (or explicitly N/A if not in tree)
- [ ] SPEC / COMPATIBILITY / COMMAND_COVERAGE / RFC plan links updated
- [ ] CHANGELOG updated
- [ ] Examples updated (generic names only)
- [ ] `./scripts/release-check` PASS
- [ ] CI green (or local equivalent documented if CI unavailable)
- [ ] Version bump plan recorded (`1.1.0-beta` or sdk note)

**Exit artifact:** `mcrpc` tag/branch ready as **1.1.0-beta**.

---

## Phase 2 DONE — MeshCore

- [ ] Heltec mcRPC env builds
- [ ] LW010 mcRPC env builds (when applicable)
- [ ] Desktop / host mcRPC tests OK
- [ ] Docker verify path OK (when used)
- [ ] Discovery emits 1.1 fields via mcrpc builders
- [ ] `@id` match on device (full id; unique prefix OK)
- [ ] No tag/role dispatch
- [ ] Examples / `doc/` use generic names
- [ ] CI / board CI green as applicable
- [ ] Depends on Phase 1 beta consumed

**Exit artifact:** MeshCore **beta** branch/tag with mcRPC 1.1 consumer.

---

## Phase 3 DONE — meshcore-ha

- [ ] HA bridge TX: identity / `@id` / `all` only
- [ ] Completions / services reject capability & tag targets
- [ ] Discovery cache stores id, caps, metadata, tag/profile
- [ ] UI tag filter → identity send
- [ ] Diagnostics expose new cache fields where appropriate
- [ ] Migration / HACS upgrade tests OK
- [ ] Chat E2E / policy / mcrpc unit suites OK
- [ ] Docs & automations: generic examples
- [ ] `./scripts/release-check` PASS
- [ ] HACS-installable schema floor still valid

**Exit artifact:** meshcore-ha **beta** suitable for lab QA.

---

## Phase 4 DONE — Android

- [ ] Completions: identities + `all` (+ `@` short forms)
- [ ] No tag/capability send chips
- [ ] Request-id remains `name#digits`
- [ ] Contact/metadata shows caps/tag/id when available
- [ ] App CI / instrumentation as applicable
- [ ] Compatible with 1.0 and 1.1 discovery lines

**Exit artifact:** Android **beta** for companion QA.

---

## Train-level DONE (before RC)

- [ ] Phases 1–4 DoD complete
- [ ] Cross-phase test matrix from implementation plan executed
- [ ] No Protocol 1.0 regression in HA default TX path
- [ ] QA lab scenarios (identity, `@id`, `all`, caps filter) signed off
- [ ] RC changelog across repos aligned

## GA DONE

- [ ] RC issues closed or waived
- [ ] Stable tags published per release process
- [ ] HACS / firmware / app channels updated as required

---

*Do not mark a phase DONE based on documentation alone.*
