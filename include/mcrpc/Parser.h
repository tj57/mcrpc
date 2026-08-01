#pragma once

#include <mcrpc/McRpcTypes.h>

namespace mcrpc {

/**
 * Pure text parser for mcRPC messages.
 *
 * Grammar (see PROTOCOL / mcRPC-CORE):
 *   message = target [ "#" DIGITS ] SP command *(SP argument)
 *
 * Never touches hardware. Case-insensitive commands; args preserve case.
 */
class Parser {
public:
  /** Parse one mcRPC request line into @p out. */
  static ParseResult parse(const char* input, Request& out);

  /**
   * Strip optional chat-style `Name: ` prefix (whitespace required after `:`).
   * Leaves protocol forms such as `group:name` unchanged.
   * Returns a pointer into @p text (or "").
   */
  static const char* stripSenderPrefix(const char* text);
};

}  // namespace mcrpc
