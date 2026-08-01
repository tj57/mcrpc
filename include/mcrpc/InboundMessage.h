#pragma once

#include <mcrpc/McRpcTypes.h>

namespace mcrpc {

/**
 * @file InboundMessage.h
 * @brief Transport-agnostic inbound envelope.
 *
 * The transport adapter unwraps framing (radio, BLE, MQTT, UART, …) into
 * plain text before calling the library. Optional metadata is advisory.
 */
struct InboundMessage {
  const char* text = nullptr;  // may include chat-style "Name: " prefix
  int8_t rssi = 0;
  uint8_t channel_hash = 0;
  bool has_rssi = false;
};

/** Result of turning inbound text into a command object. */
struct CommandObject {
  Request request;
  ParseResult parse_result = ParseResult::Empty;
  bool addressed = false;
};

}  // namespace mcrpc
