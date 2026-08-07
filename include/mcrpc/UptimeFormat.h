#pragma once

/**
 * Human-readable uptime for discovery/status (`up=1h33m`).
 * Also parses legacy `uptime=<seconds>` when needed by clients (C++ helper).
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace mcrpc {

/**
 * Format seconds into compact human form (max two segments).
 * Examples: 45s, 12m, 1h33m, 2d4h, 0s
 * Writes into out (NUL-terminated). Returns false if out_size too small.
 */
inline bool formatUptime(uint32_t seconds, char* out, size_t out_size) {
  if (!out || out_size < 4) return false;

  if (seconds < 60u) {
    return snprintf(out, out_size, "%lus", (unsigned long)seconds) > 0;
  }
  if (seconds < 3600u) {
    unsigned long m = seconds / 60u;
    unsigned long s = seconds % 60u;
    if (s == 0) return snprintf(out, out_size, "%lum", m) > 0;
    return snprintf(out, out_size, "%lum%lus", m, s) > 0;
  }
  if (seconds < 86400u) {
    unsigned long h = seconds / 3600u;
    unsigned long m = (seconds % 3600u) / 60u;
    if (m == 0) return snprintf(out, out_size, "%luh", h) > 0;
    return snprintf(out, out_size, "%luh%lum", h, m) > 0;
  }
  unsigned long d = seconds / 86400u;
  unsigned long h = (seconds % 86400u) / 3600u;
  if (h == 0) return snprintf(out, out_size, "%lud", d) > 0;
  return snprintf(out, out_size, "%lud%luh", d, h) > 0;
}

/** Copy first 8 hex chars of id to out[9], lowercase. Pads/truncates. */
inline bool shortId8(const char* full_hex, char* out9) {
  if (!out9) return false;
  out9[0] = 0;
  if (!full_hex || !full_hex[0]) return false;
  size_t n = 0;
  while (full_hex[n] && n < 8) {
    char c = full_hex[n];
    if (c >= 'A' && c <= 'F') c = (char)(c - 'A' + 'a');
    out9[n++] = c;
  }
  out9[n] = 0;
  return n > 0;
}

}  // namespace mcrpc
