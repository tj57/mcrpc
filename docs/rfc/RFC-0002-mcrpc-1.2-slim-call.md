# RFC-0002: mcRPC Protocol 1.2 — slim discovery, rich status, namespaced `call`

- **Status:** Accepted (implementation target)
- **Protocol:** 1.2 (`v=1.2` on the wire)
- **Supersedes (additive):** discovery shape from RFC-0001 §5 where conflicting
- **Compatibility:** Clients MUST ignore unknown `key=value` fields (see §6)

---

## 1. Motivation

MeshCore group-text bodies are ~155 bytes after the sender prefix. Protocol 1.1
discovery packed full 64-hex `id=`, four version fields, `features=`, often
`transport=` and both `tag=`/`profile=`, which truncated useful `caps=`.

Protocol 1.2 splits **identity for addressing** (short discovery) from **full
device model** (status), and adds a real RPC verb `call` with namespaced
procedures and a closed result grammar.

---

## 2. Discovery 1.2 (slim)

### 2.1 Emitter MUST emit (in this order when present)

```text
<name> id=<8hex> fw=<string> v=1.2 [tag=<ui>] [up=<human>] [caps=<csv>]
```

| Field | Rule |
|-------|------|
| `id=` | Exactly **8** lowercase hex chars (prefix of full pubkey hex) |
| `fw=` | Application / image version string |
| `v=` | Wire protocol major.minor (`1.2`) |
| `tag=` | UI hint only — **not** an RF address |
| `up=` | Human uptime (`45s`, `12m`, `1h33m`, `2d4h`); max two segments |
| `caps=` | Canonical CSV (lowercase, unique, sorted) as in RFC-0001 §5.5 |

### 2.2 Emitter MUST NOT emit on discovery

`protocol`, `protocol_min`, `protocol_max`, `sdk`, `features=`, `transport=`,
`profile=`, full-length `id=` (64 hex), raw `uptime=<seconds>` as the only form.

### 2.3 Forward compatibility

> **Unknown fields in `discovery` responses MUST be ignored.**

---

## 3. Status 1.2 (rich)

Status is the detailed device model. Minimum when the host can supply them:

```text
status name=… id=… id_full=… fw=… v=… [tag=…] [up=…]
```

Recommended:

```text
rssi= snr= battery= voltage= charging= heap= transport=meshcore [caps=…]
```

Features MAY contribute additional keys (`temperature=`, …).

> **Unknown fields in `status` responses MUST be ignored.**

---

## 4. Command `call` (namespaced RPC)

### 4.1 Parser neutrality

> **`call` MUST NOT change parser state.**

`ha#7 call scene.morning` is a normal request: `command=call`,
`args[0]=scene.morning`, further args are tokens (typically `key=value`).
The parser MUST NOT validate namespaces or enter an “RPC mode”.
The **dispatcher** / registered `"call"` handler validates and routes.

### 4.2 Procedure name

```text
proc = ns "." action
ns   = ALPHA *ALPHANUM_UNDER
action = ALPHA *ALPHANUM_UNDER
```

Emit lowercase. Exactly one dot. Flat names (`button_pressed`) and multi-dot
paths (`button.press.v2`) are invalid in 1.2 (`err invalid_argument`).

Examples: `button.pressed`, `ha.notify`, `scene.morning`, `gps.fix`.

### 4.3 Call result grammar

```abnf
call-result = "ok"   *(SP kv)
            / "err"  SP err-code *(SP kv)
            / "busy" *(SP kv)
            / "retry" *(SP kv)
kv          = key "=" value
err-code    = "unsupported" / "unknown_proc" / "invalid_argument"
            / "denied" / "timeout" / "internal"
            ; later RFCs MAY add codes; clients treat unknown codes as errors
```

**Payload after the result word MUST be only `key=value` tokens.**

Allowed:

```text
#7 ok
#7 ok lat=50 lon=19
#7 ok battery=92
#8 err unsupported feature=gps
#8 err denied reason=acl
#9 busy retry_after=5
#10 retry
```

Forbidden (MUST NOT):

```text
ok button pressed
ok scene started
err gps not available
busy try later
success
done
```

Unknown keys in a well-formed result MUST be ignored by clients.

This grammar applies **only** to replies to `call`. Other commands keep their
existing shapes (`pong`, `gps lat=…`, …).

### 4.4 Events

New emitters SHOULD use dotted names aligned with procedures
(`event button.pressed count=4`). Legacy flat names MAY be accepted by
receivers during transition.

---

## 5. Versioning

| Axis | 1.2 value |
|------|-----------|
| Wire `v=` | `1.2` |
| SDK / library (informative) | `1.2.2` |

Legacy `protocol=` / `sdk=` in discovery are obsolete for new emitters; readers
MAY still parse them from transitional peers.

---

## 6. Compatibility summary

| Rule | Requirement |
|------|-------------|
| Unknown discover fields | MUST ignore |
| Unknown status fields | MUST ignore |
| Unknown call-result keys | MUST ignore |
| Addressing (`@id`, name, all) | Unchanged from RFC-0001 |
| Tag / profile as RF address | Still forbidden |
| Reply to `all` | MUST stagger TX (§8) |

---

## 7. Examples

```text
all discovery
→ button id=3cbbf74e tag=ha fw=2.11.0 v=1.2 up=1h33m caps=battery,button

button status
→ status name=button id=3cbbf74e id_full=3cbbf74e1feef235… fw=2.11.0 v=1.2
   tag=ha up=1h33m rssi=-91 snr=7 heap=48200 transport=meshcore caps=battery,button

ha#7 call button.pressed count=4
→ #7 ok

ha#8 call scene.nope
→ #8 err unknown_proc

event button.pressed count=4
```

---

## 8. Broadcast reply stagger (half-duplex)

When multiple peers answer `all <cmd>` on a shared half-duplex radio (MeshCore
group text), simultaneous TX collides and a transmitting node cannot RX peer
replies. Android (idle RX) may see every answer while a companion that also
answered may miss the others.

### 8.1 Requirement

> **Emitters that transmit a reply to a request with `AddressKind::All` MUST
> delay that TX** by a non-zero stagger drawn from the window below.
>
> Addressed replies (`Named` / `@id` / `Self` / `Group`) MUST NOT use the
> broadcast window (MAY use a short ≤120 ms jitter).
>
> Spontaneous **events** MUST NOT use the broadcast window (they are not
> answers to `all`).

### 8.2 Recommended algorithm (library `ReplyJitter`)

```text
BROADCAST_MIN_MS = 250
BROADCAST_MAX_MS = 1750
SLOT_COUNT       = 8
slot             = FNV-1a32(identity) % SLOT_COUNT
slot_w           = (BROADCAST_MAX_MS - BROADCAST_MIN_MS) / SLOT_COUNT
delay_ms         = BROADCAST_MIN_MS + slot * slot_w + random(0, slot_w)
```

`identity` SHOULD be the node’s full hex id when known, else the stable node
name. Transports apply `delay_ms` (sleep, `sendFlood(pkt, delay_ms)`, …).

Reference: `include/mcrpc/ReplyJitter.h`, Python `mcrpc.reply_jitter`.

### 8.3 Compatibility

Wire grammar is unchanged. This is an **emitter timing** rule for protocol 1.2
peers on shared RF. Clients MUST NOT assume a fixed answer latency for `all`.
