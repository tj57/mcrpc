# Command coverage matrix (RC)

Status legend: ✓ present · △ partial · — absent · n/a not applicable

## Core (mandatory) commands

| Command | Parser | Dispatcher | Unit | Golden | Compliance | Python | HA | Chat E2E |
|---------|--------|------------|------|--------|------------|--------|-----|----------|
| `ping` | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `status` | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `discovery` / `discover` | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `help` | ✓ | ✓ | ✓ | △ | ✓ | ✓ | ✓ | ✓ |
| `caps` | ✓ | ✓ | ✓ | △ | ✓ | ✓ | ✓ | ✓ |

## Error contract (SPEC §18)

| Case | Wire | Unit | Golden | Compliance | Python | HA | Chat E2E |
|------|------|------|--------|------------|--------|-----|----------|
| Unknown verb | `err unknown_command` | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| Known, unavailable (`relay` / `battery` / `gps`) | `err unsupported` | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| Not addressed | *(no reply)* | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| Public while `listen=[mcCtrl]` | *(absolute silence)* | n/a | n/a | n/a | n/a | ✓ | ✓ |

## Optional feature commands

| Command | Parser | Dispatcher | Unit | Golden | Compliance | Python | HA | Chat |
|---------|--------|------------|------|--------|------------|--------|-----|------|
| `battery` / `voltage` / `charging` | ✓ | ✓ → unsupported if no host | ✓ | ✓ unsupported | ✓ | ✓ | ✓ unsupported | ✓ |
| `gps` / `location` / `track` | ✓ | ✓ (gps_no_fix / unsupported / busy) | ✓ | ✓ unsupported stub | ✓ | ✓ | ✓ unsupported | ✓ |
| `relay` / `toggle` / `power` | ✓ | ✓ always unsupported stub | ✓ | ✓ | ✓ | ✓ | ✓ | — |

## Stress / TX coverage

| Scenario | Covered by |
|----------|------------|
| Companion `NOT_FOUND` vs `TABLE_FULL` | firmware + HA `tx_pipeline` classification |
| Reply queue under radio busy | `McRpcMesh` queue + counters |
| Airtime / pool limits | `doc/MCRPC_STRESS_TX_ANALYSIS.md` |
| Burst Chat on mcCtrl | Manual QA + diagnostics download |

## Gaps closed this RC

1. HA returned `unknown_command` for `battery`/`gps` → **`unsupported`**.
2. Public traffic still entered parse/trace when `listen=[1]` → **early absolute ignore**.
3. Coverage matrix includes HA / Chat / Stress columns.
