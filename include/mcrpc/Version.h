#pragma once

/**
 * @file Version.h
 * @brief Independent versioning for protocol, SDK API, and package release.
 *
 * Protocol 1.2 (RFC-0002): slim discovery, rich status, namespaced call, reply stagger.
 * Library 1.2.3: wider ReplyJitter window (16 slots, 400–3600 ms) + short-id slot seed.
 */

namespace mcrpc {

#ifndef MCRPC_PROTOCOL_VERSION_MAJOR
#define MCRPC_PROTOCOL_VERSION_MAJOR 1
#endif
#ifndef MCRPC_PROTOCOL_VERSION_MINOR
#define MCRPC_PROTOCOL_VERSION_MINOR 2
#endif
#ifndef MCRPC_PROTOCOL_VERSION_STRING
#define MCRPC_PROTOCOL_VERSION_STRING "1.2"
#endif

#ifndef MCRPC_SDK_VERSION_MAJOR
#define MCRPC_SDK_VERSION_MAJOR 1
#endif
#ifndef MCRPC_SDK_VERSION_MINOR
#define MCRPC_SDK_VERSION_MINOR 2
#endif
#ifndef MCRPC_SDK_VERSION_PATCH
#define MCRPC_SDK_VERSION_PATCH 3
#endif
#ifndef MCRPC_SDK_VERSION_STRING
#define MCRPC_SDK_VERSION_STRING "1.2.3"
#endif

#ifndef MCRPC_LIBRARY_VERSION_MAJOR
#define MCRPC_LIBRARY_VERSION_MAJOR 1
#endif
#ifndef MCRPC_LIBRARY_VERSION_MINOR
#define MCRPC_LIBRARY_VERSION_MINOR 2
#endif
#ifndef MCRPC_LIBRARY_VERSION_PATCH
#define MCRPC_LIBRARY_VERSION_PATCH 3
#endif
#ifndef MCRPC_LIBRARY_VERSION_STRING
#define MCRPC_LIBRARY_VERSION_STRING "1.2.3"
#endif

inline constexpr int protocolVersionCode() {
  return MCRPC_PROTOCOL_VERSION_MAJOR * 100 + MCRPC_PROTOCOL_VERSION_MINOR;
}

inline const char* protocolVersionString() { return MCRPC_PROTOCOL_VERSION_STRING; }
inline const char* sdkVersionString() { return MCRPC_SDK_VERSION_STRING; }
inline const char* libraryVersionString() { return MCRPC_LIBRARY_VERSION_STRING; }

}  // namespace mcrpc
