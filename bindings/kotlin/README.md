# Kotlin / Android bindings (design only — not implemented in 1.0)

## Recommended approach

1. Compile `libmcrpc` (+ C ABI) into an Android NDK `.so`.
2. JNI or JavaCPP thin layer → Kotlin API.
3. Keep MeshCore radio stack out of this library; Android app supplies transport.

## Proposed Kotlin surface

```kotlin
data class Request(val target: String, val requestId: Long?, val command: String, val args: List<String>)
object McRpc {
  fun parse(line: String): Request
  fun buildRequest(target: String, command: String, requestId: Long? = null, args: List<String> = emptyList()): String
  fun buildEvent(name: String, kv: String? = null): String
  val protocolVersion: String
  val sdkVersion: String
}
```

## Status

**Design only.**
