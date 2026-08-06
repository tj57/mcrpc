#pragma once

/**
 * Canonical CSV for caps= / features= (RFC-0001 §5.5):
 * lowercase, unique, alphabetically sorted, comma-separated, no spaces.
 */

#include <mcrpc/McRpcTypes.h>
#include <ctype.h>
#include <string.h>

namespace mcrpc {

#ifndef MCRPC_CSV_MAX_TOKENS
#define MCRPC_CSV_MAX_TOKENS 24
#endif

#ifndef MCRPC_CSV_TOKEN
#define MCRPC_CSV_TOKEN 32
#endif

inline void asciiLowerInPlace(char* s) {
  if (!s) return;
  for (; *s; ++s) {
    if (*s >= 'A' && *s <= 'Z') *s = (char)(*s - 'A' + 'a');
  }
}

/** Write canonical CSV into @p out (NUL-terminated). Returns false if overflow. */
inline bool canonicalizeCsv(const char* const* tokens, size_t n, char* out, size_t out_size) {
  if (!out || out_size == 0) return false;
  out[0] = 0;
  if (tokens == nullptr || n == 0) return true;

  char buf[MCRPC_CSV_MAX_TOKENS][MCRPC_CSV_TOKEN];
  size_t count = 0;
  for (size_t i = 0; i < n && count < MCRPC_CSV_MAX_TOKENS; ++i) {
    if (!tokens[i] || !tokens[i][0]) continue;
    char tmp[MCRPC_CSV_TOKEN];
    copyToken(tmp, sizeof(tmp), tokens[i], strlen(tokens[i]));
    asciiLowerInPlace(tmp);
    bool dup = false;
    for (size_t j = 0; j < count; ++j) {
      if (strcmp(buf[j], tmp) == 0) {
        dup = true;
        break;
      }
    }
    if (dup) continue;
    copyToken(buf[count], sizeof(buf[count]), tmp, strlen(tmp));
    ++count;
  }

  // insertion sort
  for (size_t i = 1; i < count; ++i) {
    char key[MCRPC_CSV_TOKEN];
    copyToken(key, sizeof(key), buf[i], strlen(buf[i]));
    size_t j = i;
    while (j > 0 && strcmp(buf[j - 1], key) > 0) {
      copyToken(buf[j], sizeof(buf[j]), buf[j - 1], strlen(buf[j - 1]));
      --j;
    }
    copyToken(buf[j], sizeof(buf[j]), key, strlen(key));
  }

  size_t len = 0;
  for (size_t i = 0; i < count; ++i) {
    size_t tlen = strlen(buf[i]);
    size_t need = tlen + (i ? 1 : 0);
    if (len + need + 1 > out_size) return false;
    if (i) out[len++] = ',';
    memcpy(out + len, buf[i], tlen);
    len += tlen;
  }
  out[len] = 0;
  return true;
}

/** Case-insensitive hex prefix: does full_id start with prefix (both hex)? */
inline bool hexIdPrefixMatch(const char* full_id, const char* prefix) {
  if (!full_id || !prefix || !prefix[0]) return false;
  for (; *prefix; ++prefix, ++full_id) {
    if (!*full_id) return false;
    char a = *full_id, b = *prefix;
    if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
    if (b >= 'A' && b <= 'Z') b = (char)(b - 'A' + 'a');
    auto hex = [](char c) {
      return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    };
    if (!hex(a) || !hex(b) || a != b) return false;
  }
  return true;
}

inline bool isHexString(const char* s) {
  if (!s || !s[0]) return false;
  for (; *s; ++s) {
    char c = *s;
    if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
    if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) return false;
  }
  return true;
}

}  // namespace mcrpc
