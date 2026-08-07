#pragma once

/**
 * Canonical `call` result builders — payload MUST be key=value only.
 * See RFC-0002 §4.3.
 */

#include <mcrpc/McRpcTypes.h>
#include <string.h>
#include <ctype.h>

namespace mcrpc {

class CallResult {
public:
  static void ok(ReplyBuffer& out) {
    out.clear();
    out.append("ok");
  }

  static void busy(ReplyBuffer& out) {
    out.clear();
    out.append("busy");
  }

  static void retry(ReplyBuffer& out) {
    out.clear();
    out.append("retry");
  }

  /** `err <code>` — code from RFC-0002 list (or additive). */
  static void err(ReplyBuffer& out, const char* code) {
    out.clear();
    out.append("err ");
    out.append(code && code[0] ? code : "internal");
  }

  /**
   * Append ` key=value`. Rejects keys/values with spaces.
   * Call only after ok/err/busy/retry was written.
   */
  static bool appendKv(ReplyBuffer& out, const char* key, const char* value) {
    if (!key || !key[0] || !value) return false;
    if (strchr(key, ' ') || strchr(key, '=')) return false;
    if (strchr(value, ' ')) return false;
    if (!out.appendChar(' ')) return false;
    if (!out.append(key)) return false;
    if (!out.appendChar('=')) return false;
    return out.append(value);
  }

  /**
   * Validate `ns.action`: one dot, both sides [a-z][a-z0-9_]*.
   * Accepts uppercase on input (caller should still emit lowercase).
   */
  static bool isValidProc(const char* proc) {
    if (!proc || !proc[0]) return false;
    const char* dot = strchr(proc, '.');
    if (!dot || dot == proc || dot[1] == 0) return false;
    if (strchr(dot + 1, '.')) return false;  // multi-dot forbidden
    if (!isProcSegment(proc, dot)) return false;
    return isProcSegment(dot + 1, nullptr);
  }

private:
  static bool isProcSegment(const char* start, const char* end_or_null) {
    if (!start || !*start) return false;
    char c0 = *start;
    if (!((c0 >= 'a' && c0 <= 'z') || (c0 >= 'A' && c0 <= 'Z'))) return false;
    for (const char* p = start + 1; *p && p != end_or_null; ++p) {
      char c = *p;
      if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
            c == '_')) {
        return false;
      }
    }
    return true;
  }
};

}  // namespace mcrpc
