#include <mcrpc/McRpc.h>
#include <mcrpc/Version.h>
#include <mcrpc/CanonicalCsv.h>

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
  if (!_dispatcher.dispatch(line, reply)) return false;
  return publishRaw(reply.data);
}

bool McRpc::publishRaw(const char* text) {
  if (!text || !text[0]) return false;
  if (_publish) return _publish(text, _publish_ctx);
  return false;
}

void McRpc::buildStatus(StatusBuilder& status) {
  status.reset();
  status.add("name", _node_name);
  const char* tag = _tag[0] ? _tag : _profile;
  if (tag[0]) status.add("profile", tag);  // legacy field for 1.0 readers
  if (_tag[0]) status.add("tag", _tag);
  status.add("fw", _firmware);
  if (_uptime_fn) status.add("uptime", (unsigned long)_uptime_fn(_id_ctx));
  if (_rssi_fn) status.add("rssi", _rssi_fn(_id_ctx));
  _features.collectStatus(status);
}

void McRpc::buildDiscover(DiscoverBuilder& discover) {
  discover.reset();
  discover.setNodeName(_node_name);
  if (_node_id[0]) discover.add("id", _node_id);

  const char* primary_tag = _tag[0] ? _tag : _profile;
  if (primary_tag[0]) {
    discover.add("profile", primary_tag);  // legacy 1.0
    if (_tag[0]) discover.add("tag", _tag);
  }

  discover.add("fw", _firmware);
  if (_uptime_fn) discover.add("uptime", (int)_uptime_fn(_id_ctx));

  discover.add("protocol", protocolVersionString());
  discover.add("protocol_min", "1.0");
  discover.add("protocol_max", protocolVersionString());
  discover.add("sdk", sdkVersionString());

  const char* feature_tokens[] = {
      "caps-in-discovery",
      "id-addr",
      "request-id",
  };
  char features_csv[96];
  if (canonicalizeCsv(feature_tokens, 3, features_csv, sizeof(features_csv))) {
    discover.add("features", features_csv);
  }

  if (_capabilities.count() > 0) {
    const char* cap_ptrs[MCRPC_MAX_CAPS];
    size_t n = _capabilities.count();
    if (n > MCRPC_MAX_CAPS) n = MCRPC_MAX_CAPS;
    for (size_t i = 0; i < n; ++i) cap_ptrs[i] = _capabilities.at(i);
    char caps_csv[160];
    if (canonicalizeCsv(cap_ptrs, n, caps_csv, sizeof(caps_csv))) {
      discover.add("caps", caps_csv);
    }
  }

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
