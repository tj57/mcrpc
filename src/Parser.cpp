#include <mcrpc/Parser.h>
#include <mcrpc/CanonicalCsv.h>

#include <ctype.h>
#include <stdlib.h>

namespace mcrpc {

static void skipSpaces(const char*& p) {
  while (*p == ' ' || *p == '\t') ++p;
}

static bool isIdentChar(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
         (c >= '0' && c <= '9') || c == '_' || c == '-';
}

static bool isHexChar(char c) {
  if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

static bool readToken(const char*& p, char* dest, size_t dest_size, bool ident_only) {
  skipSpaces(p);
  if (*p == 0) return false;
  const char* start = p;
  if (ident_only) {
    while (isIdentChar(*p)) ++p;
  } else {
    while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') ++p;
  }
  if (p == start) return false;
  copyToken(dest, dest_size, start, (size_t)(p - start));
  return true;
}

const char* Parser::stripSenderPrefix(const char* text) {
  if (text == nullptr) return "";
  // MeshCore group text is "<sender name>: <payload>". Sender names may contain
  // spaces (e.g. "Home Assistant: all ping"). Require ": " so protocol forms
  // like "group:sensors" are left untouched.
  const char* colon = strchr(text, ':');
  if (colon == nullptr) return text;
  const char* msg = colon + 1;
  if (*msg != ' ' && *msg != '\t') return text;
  while (*msg == ' ' || *msg == '\t') ++msg;
  return msg;
}

ParseResult Parser::parse(const char* input, Request& out) {
  memset(&out, 0, sizeof(out));
  if (input == nullptr) return ParseResult::Empty;

  const char* p = input;
  skipSpaces(p);
  while (*p == '\r' || *p == '\n') ++p;
  if (*p == 0) return ParseResult::Empty;

  skipSpaces(p);
  if (*p == 0) return ParseResult::MissingTarget;

  // RFC-0001: @hex identity — '#' + digits remains request-id only
  if (*p == '@') {
    ++p;
    const char* start = p;
    while (isHexChar(*p)) ++p;
    if (p == start) return ParseResult::Malformed;
    copyToken(out.target, sizeof(out.target), start, (size_t)(p - start));
    out.address_kind = AddressKind::Id;
  } else if (strncmp(p, "group:", 6) == 0 || strncmp(p, "GROUP:", 6) == 0) {
    out.address_kind = AddressKind::Group;
    p += 6;
    if (!readToken(p, out.target, sizeof(out.target), true)) {
      return ParseResult::Malformed;
    }
  } else {
    if (!readToken(p, out.target, sizeof(out.target), true)) {
      return ParseResult::Malformed;
    }
    if (ieq(out.target, "all")) {
      out.address_kind = AddressKind::All;
    } else if (ieq(out.target, "self")) {
      out.address_kind = AddressKind::Self;
    } else {
      out.address_kind = AddressKind::Named;
    }
  }

  {
    char* hash = strchr(out.target, '#');
    if (hash != nullptr && hash[1] != 0) {
      bool all_digits = true;
      for (const char* d = hash + 1; *d; ++d) {
        if (!isdigit((unsigned char)*d)) {
          all_digits = false;
          break;
        }
      }
      if (all_digits) {
        out.has_request_id = true;
        out.request_id = (uint32_t)strtoul(hash + 1, nullptr, 10);
        *hash = 0;
      }
    }
  }

  skipSpaces(p);
  if (*p == '#') {
    ++p;
    char idbuf[16];
    size_t i = 0;
    while (isdigit((unsigned char)*p) && i + 1 < sizeof(idbuf)) {
      idbuf[i++] = *p++;
    }
    idbuf[i] = 0;
    if (i == 0) return ParseResult::Malformed;
    // Digits-only request id. If non-digit hex follows without digits, malformed.
    out.has_request_id = true;
    out.request_id = (uint32_t)strtoul(idbuf, nullptr, 10);
  }

  if (!readToken(p, out.command, sizeof(out.command), true)) {
    return ParseResult::MissingCommand;
  }

  for (char* c = out.command; *c; ++c) {
    if (*c >= 'A' && *c <= 'Z') *c = (char)(*c - 'A' + 'a');
  }
  if (strcmp(out.command, "discover") == 0) {
    copyToken(out.command, sizeof(out.command), "discovery", 9);
  }

  out.argc = 0;
  while (out.argc < MCRPC_MAX_ARGS) {
    if (!readToken(p, out.args[out.argc], sizeof(out.args[out.argc]), false)) break;
    out.argc++;
  }

  return ParseResult::Ok;
}

}  // namespace mcrpc
