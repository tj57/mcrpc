#pragma once

#include <mcrpc/McRpcTypes.h>
#include <mcrpc/CommandRegistry.h>
#include <mcrpc/Parser.h>

namespace mcrpc {

/**
 * Addressing policy + registry lookup.
 * Does not know about radios, boards, or MeshCore packets.
 */
class Dispatcher {
public:
  explicit Dispatcher(CommandRegistry& registry) : _registry(registry) {}

  void setNodeName(const char* name) { _node_name = name ? name : ""; }
  /** Full hex node id (no '@'). Used for AddressKind::Id matching. */
  void setNodeId(const char* id) { _node_id = id ? id : ""; }
  void setGroupName(const char* name) { _group_name = name ? name : ""; }
  void setUserData(void* user) { _user = user; }

  /**
   * Process one inbound mcRPC line (chat sender prefix already stripped if any).
   * Returns true if a reply was written into `reply` (caller must transmit).
   * Returns false if the message was ignored (not addressed to us / empty).
   *
   * When ``out_req`` is non-null and the line parses, it receives the Request
   * (including AddressKind) so the transport can apply RFC-0002 reply stagger.
   */
  bool dispatch(const char* line, ReplyBuffer& reply, Request* out_req = nullptr);

  CommandRegistry& registry() { return _registry; }

private:
  bool isAddressedToUs(const Request& req) const;
  void writePrefixed(ReplyBuffer& reply, const Request& req, const char* body);

  CommandRegistry& _registry;
  const char* _node_name = "";
  const char* _node_id = "";
  const char* _group_name = "";
  void* _user = nullptr;
};

}  // namespace mcrpc
