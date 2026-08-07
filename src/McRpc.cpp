#include <mcrpc/McRpc.h>
#include <mcrpc/Version.h>
#include <mcrpc/CanonicalCsv.h>
#include <mcrpc/UptimeFormat.h>

#include <stdio.h>

namespace mcrpc {

void McRpc::begin() {
  _events.subscribe(&McRpc::onEventThunk, this);
  _features.start(_commands, _capabilities, _events);
}

void McRpc::loop() { _features.loop(); }

void McRpc::shutdown() {
  _features.stop();
  _events.clear();
}

bool McRpc::handleIncomingText(const char* text) {
  InboundMessage msg;
  msg.text = text;
  return handleInbound(msg);
}

bool McRpc::handleInbound(const InboundMessage& msg) {
  if (msg.text == nullptr) return false;
  const char* line = Parser::stripSenderPrefix(msg.text);
  ReplyBuffer reply;
  Request req;
  if (!_dispatcher.dispatch(line, reply, &req)) return false;
  // RFC-0002 §8: stagger replies to ``all`` before TX.
  uint32_t delay = ReplyJitter::delayMsFor(req, identityForJitter(), entropy16());
  return publishRaw(reply.data, delay);
}

bool McRpc::publishRaw(const char* text) { return publishRaw(text, 0); }

bool McRpc::publishRaw(const char* text, uint32_t delay_ms) {
  if (!text || !text[0]) return false;
  if (_publish_ex) return _publish_ex(text, delay_ms, _publish_ex_ctx);
  if (_publish) return _publish(text, _publish_ctx);
  (void)delay_ms;
  return false;
}

const char* McRpc::identityForJitter() const {
  if (_node_id && _node_id[0]) return _node_id;
  return _node_name ? _node_name : "";
}

uint16_t McRpc::entropy16() const {
  if (_entropy_fn) return _entropy_fn(_entropy_ctx);
  // Deterministic fallback: still staggers peers via identity hash slots.
  return 0;
}

void McRpc::buildStatus(StatusBuilder& status) {
  status.reset();
  status.add("name", _node_name);

  char id8[9];
  if (_node_id[0] && shortId8(_node_id, id8)) {
    status.add("id", id8);
    status.add("id_full", _node_id);
  }

  status.add("fw", _firmware);
  status.add("v", protocolVersionString());

  const char* tag = _tag[0] ? _tag : _profile;
  if (tag[0]) status.add("tag", tag);

  if (_uptime_fn) {
    char up[16];
    if (formatUptime(_uptime_fn(_id_ctx), up, sizeof(up))) status.add("up", up);
  }
  if (_rssi_fn) status.add("rssi", _rssi_fn(_id_ctx));

  if (_transport[0]) status.add("transport", _transport);

  if (_capabilities.count() > 0) {
    const char* cap_ptrs[MCRPC_MAX_CAPS];
    size_t n = _capabilities.count();
    if (n > MCRPC_MAX_CAPS) n = MCRPC_MAX_CAPS;
    for (size_t i = 0; i < n; ++i) cap_ptrs[i] = _capabilities.at(i);
    char caps_csv[96];
    if (canonicalizeCsv(cap_ptrs, n, caps_csv, sizeof(caps_csv))) {
      status.add("caps", caps_csv);
    }
  }

  _features.collectStatus(status);
}

void McRpc::buildDiscover(DiscoverBuilder& discover) {
  discover.reset();
  discover.setNodeName(_node_name);

  // RF-critical fields first (MeshCore group text ~155 body bytes after prefix).
  char id8[9];
  if (_node_id[0] && shortId8(_node_id, id8)) {
    discover.add("id", id8);
  }

  discover.add("fw", _firmware);
  discover.add("v", protocolVersionString());

  const char* primary_tag = _tag[0] ? _tag : _profile;
  if (primary_tag[0]) discover.add("tag", primary_tag);

  if (_uptime_fn) {
    char up[16];
    if (formatUptime(_uptime_fn(_id_ctx), up, sizeof(up))) discover.add("up", up);
  }

  if (_capabilities.count() > 0) {
    const char* cap_ptrs[MCRPC_MAX_CAPS];
    size_t n = _capabilities.count();
    if (n > MCRPC_MAX_CAPS) n = MCRPC_MAX_CAPS;
    for (size_t i = 0; i < n; ++i) cap_ptrs[i] = _capabilities.at(i);
    char caps_csv[96];
    if (canonicalizeCsv(cap_ptrs, n, caps_csv, sizeof(caps_csv))) {
      discover.add("caps", caps_csv);
    }
  }

  // Features SHOULD NOT pad discovery; prefer status / caps=.
  _features.collectDiscover(discover);
}

void McRpc::buildCaps(ReplyBuffer& reply) { _features.writeCapabilities(reply); }

void McRpc::buildHelp(ReplyBuffer& reply) { _features.writeHelp(reply, _commands); }

void McRpc::onEventThunk(const char* name, const char* kv, void* ctx) {
  static_cast<McRpc*>(ctx)->onEvent(name, kv);
}

void McRpc::onEvent(const char* name, const char* kv) {
  ReplyBuffer buf;
  buf.clear();
  buf.append("event ");
  buf.append(name);
  if (kv && kv[0]) {
    buf.appendChar(' ');
    buf.append(kv);
  }
  publishRaw(buf.data);
}

}  // namespace mcrpc
