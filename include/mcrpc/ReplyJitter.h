#pragma once

/**
 * @file ReplyJitter.h
 * @brief RFC-0002 §8 — broadcast reply stagger (half-duplex collision avoidance).
 *
 * Pure computation; transports apply the delay (sleep / sendFlood delay_ms).
 * Does not sleep and does not touch radios.
 *
 * Window is sized for LoRa group-text airtime: ~16 slots over ~0.4–3.6 s so
 * two peers rarely share a TX window on a half-duplex radio.
 */

#include <mcrpc/McRpcTypes.h>
#include <stddef.h>
#include <stdint.h>

namespace mcrpc {

struct ReplyJitter {
  static constexpr uint32_t BROADCAST_MIN_MS = 400;
  static constexpr uint32_t BROADCAST_MAX_MS = 3600;
  static constexpr uint32_t ADDRESSED_MAX_MS = 120;
  static constexpr uint32_t SLOT_COUNT = 16;

  static bool needsBroadcastStagger(AddressKind kind) {
    return kind == AddressKind::All;
  }

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
   * Prefer first 8 hex chars when ``identity`` looks like a pubkey hex —
   * matches discovery ``id=`` and keeps HA / firmware on the same slot basis.
   */
  static const char* slotSeed(const char* identity, char out9[9]) {
    if (!identity || !identity[0]) {
      out9[0] = 0;
      return out9;
    }
    size_t n = 0;
    bool hexish = true;
    for (const char* p = identity; *p && n < 64; ++p) {
      char c = *p;
      bool is_hex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
                    (c >= 'A' && c <= 'F');
      if (!is_hex) {
        hexish = false;
        break;
      }
      n++;
    }
    if (hexish && n >= 8) {
      for (int i = 0; i < 8; ++i) {
        char c = identity[i];
        if (c >= 'A' && c <= 'F') c = (char)(c - 'A' + 'a');
        out9[i] = c;
      }
      out9[8] = 0;
      return out9;
    }
    // Non-hex / short: use the string as-is via identityHash(identity)
    out9[0] = 0;
    return identity;
  }

  static uint32_t delayMs(bool broadcast, const char* identity, uint16_t entropy = 0) {
    if (!broadcast) {
      if (ADDRESSED_MAX_MS == 0) return 0;
      return (uint32_t)entropy % (ADDRESSED_MAX_MS + 1u);
    }
    char seed9[9];
    const char* seed = slotSeed(identity, seed9);
    const uint32_t span = BROADCAST_MAX_MS - BROADCAST_MIN_MS;
    const uint32_t slot_w = span / SLOT_COUNT;
    const uint32_t slot = identityHash(seed) % SLOT_COUNT;
    uint32_t within = 0;
    if (slot_w > 0 && entropy != 0) {
      within = (uint32_t)entropy % slot_w;
    }
    uint32_t ms = BROADCAST_MIN_MS + slot * slot_w + within;
    if (ms > BROADCAST_MAX_MS) ms = BROADCAST_MAX_MS;
    return ms;
  }

  static uint32_t delayMsFor(const Request& req, const char* identity, uint16_t entropy = 0) {
    return delayMs(needsBroadcastStagger(req.address_kind), identity, entropy);
  }
};

}  // namespace mcrpc
