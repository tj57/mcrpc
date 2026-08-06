# mcRPC Specification v1

**Status:** Stable (Protocol 1.0)

**Reference implementation:** this repository (`libmcrpc`).

---

# 1. Introduction

mcRPC is a lightweight, text-based application RPC protocol.

It is **transport-independent**. Any channel that can carry short UTF-8 text
may carry mcRPC (MeshCore group text, MQTT, UART, BLE UART, TCP, simulators, …).

Designed for:

- Home Assistant and other home-automation peers
- Embedded devices and sensors
- Android/iOS applications
- CLI and desktop tools
- Manual use by humans
- Unit tests and protocol simulators

The protocol defines only the application layer.

Routing, encryption, and link reliability are provided by the chosen transport.
MeshCore is one supported transport, not part of the protocol grammar.

---

# 2. Design Goals

Implementations MUST satisfy:

- Human readable
- Small packets
- Easy parsing
- Stateless
- Transport independent
- Backward compatible

The protocol MUST be usable without a dedicated application.

Every command should be possible to type manually.

---

# 3. Layers

mcRPC intentionally separates responsibilities.

```
Application (HA, firmware features, CLI, …)
↑
mcRPC (this specification)
↑
Transport adapter (MeshCore / MQTT / UART / …)
↑
Link (radio, IP, serial, …)
```

The transport provides (as applicable):

- encryption
- routing
- packet forwarding
- delivery to a text channel

mcRPC provides:

- addressing
- commands
- responses
- events

---

# 4. Grammar

ABNF (RFC5234 style)

```abnf
message     = target [request] SP command *(SP argument)

target      = identifier / "all" / "self" / group

group       = "group:" identifier

request     = "#" 1*DIGIT

command     = identifier

argument    = token

identifier  = 1*(ALPHA / DIGIT / "_" / "-")

token       = 1*(VCHAR)
```

Parser MUST ignore:

- repeated spaces
- leading spaces
- trailing spaces

Commands are case insensitive.

Arguments preserve original case.

---

# 5. Addressing

Examples

```
ha
tracker
relay1
sensor2
display
```

Reserved names

```
all
self
group:<name>
```

Examples

```
all ping

group:gps status

self reboot
```

---

# 6. Request ID

Optional.

```
ha ping
```

↓

```
ha pong
```

or

```
ha#42 ping
```

↓

```
ha#42 pong
```

Responses MUST preserve Request ID.

---

# 7. Core Commands (Mandatory)

Every mcRPC device MUST implement:

```
ping
status
discovery
help
caps
```

Meaning

| Command | Description |
|----------|-------------|
| ping | Connectivity test |
| status | Current state |
| discovery | Basic information |
| help | Supported commands |
| caps | Supported capabilities |

This guarantees interoperability.

---

# 8. Standard Commands (Optional)

Read

```
get battery

get temp

get version

get gps
```

Modify

```
set led on

set relay off

set interval 300
```

Execute

```
gps

reboot

ota

reset

beep
```

---

# 9. Responses

Success

```
ok
```

Failure

```
err timeout

err busy

err denied

err unsupported

err unknown_command

err invalid_argument

err internal
```

Data

```
battery value=97

temp value=24.1

gps lat=50.12 lon=19.93 acc=4 sat=18
```

Structured data MUST use:

```
key=value
```

---

# 10. Events

Devices MAY send asynchronous events.

Examples

```
event battery_low

event panic

event motion

event gps_fix

event button_pressed
```

Events never require requests.

---

# 11. Device Profiles

Every device SHOULD declare a profile.

Examples

```
gateway

tracker

relay

sensor

display

beacon

camera

weather

environment

lighting

switch

energy
```

Profiles define additional commands.

---

# 12. Discovery

`discover` remains a required backward-compatible alias for `discovery`.

```
all discovery
```

↓

```
ha profile=gateway fw=2026.8 protocol=1.0 sdk=1.0.0

tracker profile=tracker fw=1.2 protocol=1.0 sdk=1.0.0

relay1 profile=relay fw=1.0 protocol=1.0 sdk=1.0.0
```

Required fields (after node name):

```
profile=
fw=
protocol=
sdk=
```

| Field | Meaning |
|-------|---------|
| `protocol` | Wire protocol version (`major.minor`), e.g. `1.0` |
| `sdk` | Library/SDK version (semver), e.g. `1.0.0` |

Clients MUST ignore unknown discover fields (forward compatible).

---

# 13. Status

Minimum required fields

```
status

name=

profile=

fw=

uptime=

rssi=
```

Recommended

```
battery=

mesh=

voltage=

temp=
```

---

# 14. Capabilities

Example

```
caps

gps

battery

ota

led

relay

temperature
```

Capabilities MUST be one per line.

---

# 15. Standard Error Codes

```
timeout

busy

denied

unsupported

unknown_command

invalid_argument

internal

gps_no_fix

low_battery
```

Response format

```
err timeout
```

---

# 16. Human Interface

Protocol SHOULD remain comfortable for humans.

Good

```
tracker gps

ha sw1 on

gate open
```

Bad

```
rpc.execute.device.command(...)
```

---

# 17. Reserved Keywords

Reserved

```
all

self

group

event

ok

err

status

discovery

caps

help

ping

get

set
```

Applications MUST NOT redefine these.

---

# 18. Forward Compatibility

Unknown **commands** (no handler registered)

↓

```
err unknown_command
```

Known command / capability that is not available on this device

↓

```
err unsupported
```

Unknown fields in status/discovery/events

↓

Ignored.

Future fields MUST NOT break existing parsers.

---

# 19. Device Profiles

## Gateway

Recommended

```
status

discovery

caps

ping

scene

automation

entity
```

---

## Tracker

Recommended

```
gps

battery

sleep

wake

reboot
```

---

## Relay

Recommended

```
relay

status

power

energy
```

---

## Sensor

Recommended

```
temp

humidity

pressure

battery
```

---

# 20. Example Session

```
User

ha ping

↓

ha pong
```

```
tracker#18 gps

↓

tracker#18 gps lat=50.12 lon=19.93 acc=4 sat=18
```

```
all discovery

↓

ha profile=gateway fw=2026.8

tracker profile=tracker fw=1.2

relay1 profile=relay fw=1.0
```

```
relay1 set relay on

↓

ok
```

---

# 21. Implementation Requirements

Every compliant implementation MUST:

✓ support UTF-8

✓ support Core Commands

✓ preserve Request ID

✓ ignore unknown fields

✓ ignore repeated whitespace

✓ return standard errors

✓ support key=value format

✓ never require JSON

✓ never require XML

✓ never depend on transport layer features
---

# 22. MeshCore Transport Binding (Implementation)

This section is informational and does not change the application grammar.

mcRPC messages travel as MeshCore `PAYLOAD_TYPE_GRP_TXT` on a private group channel:

1. Channel name + 16-byte PSK configured in `/mcrpc_cfg` (or build defaults)
2. Channel hash = first byte of SHA-256(PSK)
3. Wire payload: `timestamp(4) | TXT_TYPE_PLAIN | "NodeName: <mcRPC line>"`

Inbound handlers strip the MeshCore `NodeName: ` prefix before parsing.

Outbound replies are published the same way. When a request id is present, the mcRPC body is `#<id> <response>` (after the MeshCore sender prefix). Example on air:

```
tracker: #18 gps lat=50.12 lon=19.93
```

After sender-prefix strip, consumers see `#18 gps lat=…`, which preserves correlation.

## Listening scope

Firmware decrypts only the configured channel (via `searchChannelsByHash`). Other channels are ignored.

## Home Assistant

HA is a peer that joins the same channel (companion app, MQTT bridge, etc.). The firmware never embeds MQTT/HTTP.

---

# 23. Proposed protocol improvements (not implemented)

Documented for discussion — **not silently applied** beyond what RFC-0001
ships in library betas:

| Idea | Benefit | Cost | Status |
|------|---------|------|--------|
| Identity / `@id` / `all`; caps + metadata; `tag=` | Clear fleet model | Spec + client UX | **RFC-0001** — implementing (Phase 1+) |
| Emit bare mcRPC without MeshCore `Sender:` prefix | Cleaner lines | Breaks chat UX in companion apps | deferred |
| Multi-channel listen list | Gateways | More flash + config | deferred |
| `err unsupported` vs `unknown_command` | Spec clarified in §15/§18; dispatcher uses `unknown_command` | — | closed |
| Binary TLV companion codec | Efficiency | Violates "human readable" goal | rejected |
| Glob / multi-target addressing | Fleet ops | Grammar break risk | deferred to v2 (see RFC-0001) |

---

# 24. Protocol 1.1 additives (RFC-0001)

Normative summary — full text in `docs/rfc/RFC-0001-mcrpc-1.1.md`.

**Addressing:** `name` | `@hex` | `all` (plus legacy `self` / `group:`).
Capabilities and UI tags are **not** addresses. Request id remains `name#digits`.

**Discovery:** devices SHOULD advertise full `id=`, canonical `caps=` /
`features=`, `uptime=`, prefer `tag=` (legacy `profile=` allowed).

**CSV:** lowercase, unique, alphabetically sorted, no spaces.

Clients MUST ignore unknown fields (unchanged).

