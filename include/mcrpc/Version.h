#pragma once

/**
 * @file Version.h
 * @brief Independent versioning for protocol, SDK API, and package release.
 *
 * | Axis | Meaning | Bump when |
 * |------|---------|-----------|
 * | Protocol | Wire grammar / semantics | Breaking or additive wire change |
 * | SDK | C++ public API | Breaking or additive API change |
 * | Library | Packaged release (git tag) | Any published release |
 *
 * Compatibility policy: an SDK 1.x build speaks protocol 1.y when discover
 * advertises matching major. Clients MUST ignore unknown discover fields.
 */

namespace mcrpc {

#ifndef MCRPC_PROTOCOL_VERSION_MAJOR
#define MCRPC_PROTOCOL_VERSION_MAJOR 1
#endif
#ifndef MCRPC_PROTOCOL_VERSION_MINOR
#define MCRPC_PROTOCOL_VERSION_MINOR 0
#endif
#ifndef MCRPC_PROTOCOL_VERSION_STRING
#define MCRPC_PROTOCOL_VERSION_STRING "1.0"
#endif

#ifndef MCRPC_SDK_VERSION_MAJOR
#define MCRPC_SDK_VERSION_MAJOR 1
#endif
#ifndef MCRPC_SDK_VERSION_MINOR
#define MCRPC_SDK_VERSION_MINOR 0
#endif
#ifndef MCRPC_SDK_VERSION_PATCH
#define MCRPC_SDK_VERSION_PATCH 0
#endif
#ifndef MCRPC_SDK_VERSION_STRING
#define MCRPC_SDK_VERSION_STRING "1.0.0"
#endif

#ifndef MCRPC_LIBRARY_VERSION_MAJOR
#define MCRPC_LIBRARY_VERSION_MAJOR 1
#endif
#ifndef MCRPC_LIBRARY_VERSION_MINOR
#define MCRPC_LIBRARY_VERSION_MINOR 0
#endif
#ifndef MCRPC_LIBRARY_VERSION_PATCH
#define MCRPC_LIBRARY_VERSION_PATCH 0
#endif
#ifndef MCRPC_LIBRARY_VERSION_STRING
#define MCRPC_LIBRARY_VERSION_STRING "1.0.0"
#endif

/** Numeric protocol id: major*100 + minor (e.g. 1.0 → 100). */
inline constexpr int protocolVersionCode() {
  return MCRPC_PROTOCOL_VERSION_MAJOR * 100 + MCRPC_PROTOCOL_VERSION_MINOR;
}

inline const char* protocolVersionString() { return MCRPC_PROTOCOL_VERSION_STRING; }
inline const char* sdkVersionString() { return MCRPC_SDK_VERSION_STRING; }
inline const char* libraryVersionString() { return MCRPC_LIBRARY_VERSION_STRING; }

}  // namespace mcrpc
