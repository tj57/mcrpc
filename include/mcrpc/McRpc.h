#pragma once

#include <mcrpc/Dispatcher.h>
#include <mcrpc/FeatureManager.h>
#include <mcrpc/Parser.h>
#include <mcrpc/CommandRegistry.h>
#include <mcrpc/CapabilityRegistry.h>
#include <mcrpc/EventBus.h>
#include <mcrpc/StatusBuilder.h>
#include <mcrpc/DiscoverBuilder.h>
#include <mcrpc/InboundMessage.h>
#include <mcrpc/Config.h>
#include <mcrpc/ReplyJitter.h>

namespace mcrpc {

/** Immediate publish (events, manual TX). delay_ms is ignored. */
using PublishFn = bool (*)(const char* text, void* ctx);

/**
 * Publish with optional TX delay (RFC-0002 §8).
 * Transports SHOULD honour ``delay_ms`` (sleep / MeshCore sendFlood delay).
 */
using PublishExFn = bool (*)(const char* text, uint32_t delay_ms, void* ctx);

/** Optional entropy for ReplyJitter (return 0..65535). */
using EntropyFn = uint16_t (*)(void* ctx);

/**
 * Framework facade.
 *
 * Features depend on Feature / FeatureContext / HostServices only.
 * Transport depends on handleInbound / EventBus subscription.
 */
class McRpc {
public:
  McRpc() : _dispatcher(_commands) {}

  CommandRegistry& commands() { return _commands; }
  /** @deprecated Prefer commands(). Kept for 1.x compatibility. */
  CommandRegistry& registry() { return _commands; }
  CapabilityRegistry& capabilities() { return _capabilities; }
  EventBus& events() { return _events; }
  Dispatcher& dispatcher() { return _dispatcher; }
  FeatureManager& features() { return _features; }
  Config& config() { return _config; }

  void setPublishHandler(PublishFn fn, void* ctx) {
    _publish = fn;
    _publish_ctx = ctx;
  }

  /** Preferred when the transport can delay TX (MeshCore / HA). */
  void setPublishExHandler(PublishExFn fn, void* ctx) {
    _publish_ex = fn;
    _publish_ex_ctx = ctx;
  }

  void setEntropy(EntropyFn fn, void* ctx) {
    _entropy_fn = fn;
    _entropy_ctx = ctx;
  }

  void setNodeIdentity(const char* node_name, const char* group_name) {
    _node_name = node_name ? node_name : "";
    _dispatcher.setNodeName(_node_name);
    _dispatcher.setGroupName(group_name);
  }

  /** Full hex id (no '@'). Enables @id addressing; discovery emits 8-char prefix. */
  void setNodeId(const char* id) {
    _node_id = id ? id : "";
    _dispatcher.setNodeId(_node_id);
  }

  void setFirmwareVersion(const char* fw) { _firmware = fw ? fw : ""; }
  void setProfile(const char* profile) { _profile = profile ? profile : ""; }
  /** Preferred UI tag (RFC-0002); discovery emits tag= only (not profile=). */
  void setTag(const char* tag) { _tag = tag ? tag : ""; }
  void setTransport(const char* transport) { _transport = transport ? transport : ""; }

  using UptimeFn = uint32_t (*)(void*);
  using RssiFn = int (*)(void*);
  void setIdentityCallbacks(UptimeFn uptime, RssiFn rssi, void* ctx) {
    _uptime_fn = uptime;
    _rssi_fn = rssi;
    _id_ctx = ctx;
  }

  void setUserData(void* user) { _dispatcher.setUserData(user); }

  void begin();
  void loop();
  void shutdown();

  bool handleIncomingText(const char* text);
  bool handleInbound(const InboundMessage& msg);
  bool publishRaw(const char* text);
  /** Publish with RFC-0002 stagger delay hint (0 = immediate). */
  bool publishRaw(const char* text, uint32_t delay_ms);

  void buildStatus(StatusBuilder& status);
  void buildDiscover(DiscoverBuilder& discover);
  void buildCaps(ReplyBuffer& reply);
  void buildHelp(ReplyBuffer& reply);

private:
  static void onEventThunk(const char* name, const char* kv, void* ctx);
  void onEvent(const char* name, const char* kv);
  const char* identityForJitter() const;
  uint16_t entropy16() const;

  CommandRegistry _commands;
  CapabilityRegistry _capabilities;
  EventBus _events;
  Dispatcher _dispatcher;
  FeatureManager _features;
  Config _config;
  PublishFn _publish = nullptr;
  void* _publish_ctx = nullptr;
  PublishExFn _publish_ex = nullptr;
  void* _publish_ex_ctx = nullptr;
  EntropyFn _entropy_fn = nullptr;
  void* _entropy_ctx = nullptr;
  const char* _node_name = "";
  const char* _node_id = "";
  const char* _profile = "";
  const char* _tag = "";
  const char* _firmware = "";
  const char* _transport = "meshcore";
  UptimeFn _uptime_fn = nullptr;
  RssiFn _rssi_fn = nullptr;
  void* _id_ctx = nullptr;
};

}  // namespace mcrpc
