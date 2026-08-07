#pragma once

/**
 * @file ReplyJitter.h
 * @brief RFC-0002 §8 — broadcast reply stagger (half-duplex collision avoidance).
 *
 * Pure computation; transports apply the delay (sleep / sendFlood delay_ms).
 * Does not sleep and does not touch radios.
 */

#include <mcrpc/McRpcTypes.h>
#include <stdint.h>

namespace mcrpc {

/**
 * Recommended reply delay windows (milliseconds).
 * Emitters answering AddressKind::All MUST stagger within the broadcast window.
 */
struct ReplyJitter {
  static constexpr uint32_t BROADCAST_MIN_MS = 250;
  static constexpr uint32_t BROADCAST_MAX_MS = 1750;
  static constexpr uint32_t ADDRESSED_MAX_MS = 120;
  static constexpr uint32_t SLOT_COUNT = 8;

  /** True when a reply to this addressing MUST be staggered (RFC-0002 §8). */
  static bool needsBroadcastStagger(AddressKind kind) {
    return kind == AddressKind::All;
  }

  /** FNV-1a 32-bit over identity bytes (id hex or node name). */
  static uint32_t identityHash(const char* identity) {
    uint32_t h = 2166136261u;
    if (!identity) return h;
    for (const unsigned char* p = (const unsigned char*)identity; *p; ++p) {
      h ^= *p;
      h *= 16777619u;
    }
    return h;
  }

  /**
   * Compute TX delay before publishing a reply.
   *
   * @param broadcast  true for AddressKind::All answers
   * @param identity   node id hex or stable name (slot seed); may be null
   * @param entropy    optional 16-bit random (0..65535); 0 = deterministic slot only
   */
  static uint32_t delayMs(bool broadcast, const char* identity, uint16_t entropy = 0) {
    if (!broadcast) {
      if (ADDRESSED_MAX_MS == 0) return 0;
      return (uint32_t)entropy % (ADDRESSED_MAX_MS + 1u);
    }
    const uint32_t span = BROADCAST_MAX_MS - BROADCAST_MIN_MS;
    const uint32_t slot_w = span / SLOT_COUNT;
    const uint32_t slot = identityHash(identity) % SLOT_COUNT;
    uint32_t within = 0;
    if (slot_w > 0 && entropy != 0) {
      within = (uint32_t)entropy % slot_w;
    }
    uint32_t ms = BROADCAST_MIN_MS + slot * slot_w + within;
    if (ms > BROADCAST_MAX_MS) ms = BROADCAST_MAX_MS;
    return ms;
  }

  /** Convenience from a parsed Request. */
  static uint32_t delayMsFor(const Request& req, const char* identity, uint16_t entropy = 0) {
    return delayMs(needsBroadcastStagger(req.address_kind), identity, entropy);
  }
};

}  // namespace mcrpc
