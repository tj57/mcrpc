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
| Non-listen channel (e.g. Public when listen=[1]) | *(absolute silence)* | n/a | n/a | n/a | n/a | ✓ unit | — |

> **Public is out of scope for QA.** The HA unit row above is an automated
> regression that Public traffic is ignored — QA must not transmit on Public.

## Optional feature commands

| Command | Parser | Dispatcher | Unit | Golden | Compliance | Python | HA | Chat |
|---------|--------|------------|------|--------|------------|--------|-----|------|
| `battery` / `voltage` / `charging` | ✓ | ✓ → unsupported if no host | ✓ | ✓ unsupported | ✓ | ✓ | ✓ unsupported | ✓ |
| `gps` / `location` / `track` | ✓ | ✓ (gps_no_fix / unsupported / busy) | ✓ | ✓ unsupported stub | ✓ | ✓ | ✓ unsupported | ✓ |
| `relay` / `toggle` / `power` | ✓ | ✓ always unsupported stub | ✓ | ✓ | ✓ | ✓ | ✓ | — |

## Stress / TX coverage (realistic methodology)

Do **not** use a 100-ping burst as a pass/fail RF gate.

| Profile | Pattern | Covered by |
|---------|---------|------------|
| Warm-up | 5 req @ 1 s | Lab stress + diagnostics |
| Light load | 10 req @ 2 s | Lab stress + diagnostics |
| Continuous short | paced **5 min** | Lab soak |
| Continuous long | paced **30 min** | Lab soak |
| Companion `NOT_FOUND` vs `TABLE_FULL` | classification | firmware + HA `tx_pipeline` |
| Reply queue under radio busy | counters | `McRpcMesh` queue |
| Airtime / pool limits | analysis | `doc/MCRPC_STRESS_TX_ANALYSIS.md` |

Metrics: reply success, average RTT, queue drops, packet drops, memory, CPU.
Saturation → report `busy` / `queue_full` / `table_full` / backpressure.

Full doc: meshcore-ha `docs/STRESS_METHODOLOGY.md`.

## Gaps closed this RC

1. HA returned `unknown_command` for `battery`/`gps` → **`unsupported`**.
2. Non-listen traffic still entered parse/trace → **early absolute ignore**.
3. Coverage matrix includes HA / Chat / realistic Stress columns.
4. Negative Public QA scenario **removed** (Public out of scope).
