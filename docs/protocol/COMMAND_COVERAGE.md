# Command coverage matrix (RC)

Status legend: ✓ present · △ partial · — absent · n/a not applicable

## Core (mandatory) commands

| Command | Parser | Dispatcher | Unit | Golden | Compliance | Chat E2E (HA) |
|---------|--------|------------|------|--------|------------|---------------|
| `ping` | ✓ | ✓ CoreFeature | ✓ | ✓ `ping.txt` | ✓ | ✓ |
| `status` | ✓ | ✓ CoreFeature | ✓ | ✓ `status.txt` | ✓ | ✓ (HA answer) |
| `discovery` / `discover` | ✓ alias | ✓ CoreFeature | ✓ | ✓ `discover.txt` | ✓ | ✓ (HA answer) |
| `help` | ✓ | ✓ CoreFeature | ✓ | △ via compliance | ✓ | ✓ (HA answer) |
| `caps` | ✓ | ✓ CoreFeature | ✓ | △ via compliance | ✓ | ✓ (HA answer) |

## Error contract

| Case | Wire reply | Unit | Golden | Compliance | Chat E2E |
|------|------------|------|--------|------------|----------|
| Unknown command | `err unknown_command` | ✓ | ✓ `errors.txt` | ✓ | ✓ HA bridge |
| Unsupported feature | `err unsupported` | ✓ RelayFeature / handlers | ✓ `unsupported.txt` | ✓ | n/a (HA stub has no unsupported features) |
| Not addressed | *(no reply)* | ✓ | ✓ `errors.txt` MODE ignore | ✓ | ✓ policy deny |

## Optional feature commands (reference firmware)

| Command | Parser | Dispatcher | Unit | Golden | Compliance | Chat E2E |
|---------|--------|------------|------|--------|------------|----------|
| `gps` / `location` / `track` | ✓ | ✓ GpsFeature | ✓ | ✓ `gps.txt` | △ | — |
| `battery` / `voltage` / `charging` | ✓ | ✓ BatteryFeature | △ | — | — | — |
| `button` / `button_state` | ✓ | ✓ ButtonFeature | △ | ✓ `event_button.txt` | — | — |
| `relay` / `toggle` / `power` | ✓ | ✓ → `err unsupported` | ✓ | ✓ `unsupported.txt` | ✓ | — |
| `display` / `text` / `clear` | ✓ | ✓ DisplayFeature | — | — | — | — |
| `led` | ✓ header | not linked in default lib | — | — | — | — |

## Spec-only optional commands (no handler → `unknown_command`)

`get`, `set`, `reboot`, `ota`, `reset`, `beep`, `temp`, … — intentionally unanswered as features until implemented; contract is `err unknown_command`.

## Gaps closed in this RC

1. HA bridge answered unknown commands with `err unsupported` → fixed to `unknown_command`.
2. Golden/compliance coverage for `err unsupported` (RelayFeature) added.
3. Chat E2E asserts unknown-command answer body.
