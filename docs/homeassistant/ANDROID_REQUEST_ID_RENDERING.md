# Android rendering: `ha#123 ping` large font

## Observation

When a Chat line such as `ha#123 ping` is shown in the Android MeshCore client, the
`#123` fragment may render with a larger / heading-like font.

## Captured payload contract (mcRPC)

Transmitted request body (channel text after MeshCore `name: ` prefix):

```text
ha#123 ping
```

Parser (`mcrpc` C++/Python) splits this as:

| Field | Value |
|-------|-------|
| target | `ha` |
| request_id | `123` |
| command | `ping` |

Successful reply body:

```text
#123 pong
```

(or `ha#123 pong` when using glued identity form in examples).

## Conclusion

The **on-wire payload is correct** per SPEC § request-id grammar. mcRPC must **not**
be changed for this visual issue.

The Android client almost certainly treats `#` + digits as Markdown / rich-text
heading or list markup when rendering Chat bubbles. That is a **client UI
formatting** concern, not a protocol defect.

## Recommended client follow-up (out of scope for this RC)

- Escape or disable Markdown for mesh channel text, **or**
- Display request-id lines as monospace / preformatted, **or**
- Document that `#<id>` may look like a heading in Markdown-enabled Chat UIs.

## Raw payload checklist for QA

1. Capture companion TX frame / HA `last_tx.raw` → must contain literal `ha#123 ping`.
2. Capture RX on the node / HA `last_rx` → must contain the same characters.
3. If both match SPEC, file the font issue against the Android app, not mcRPC.
