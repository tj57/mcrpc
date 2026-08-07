#include <mcrpc/features/core/CoreFeature.h>
#include <mcrpc/CallResult.h>
#include <mcrpc/McRpc.h>

namespace mcrpc {

static CoreFeature* g_core = nullptr;

void CoreFeature::registerCommands(CommandRegistry& commands) {
  g_core = this;
  commands.registerCommand("ping", &CoreFeature::cmdPing, "connectivity test");
  commands.registerCommand("status", &CoreFeature::cmdStatus, "node status");
  commands.registerCommand("discovery", &CoreFeature::cmdDiscover, "discovery info");
  commands.registerCommand("discover", &CoreFeature::cmdDiscover, "alias for discovery");
  commands.registerCommand("help", &CoreFeature::cmdHelp, "list commands");
  commands.registerCommand("caps", &CoreFeature::cmdCaps, "list capabilities");
  // RFC-0002: ordinary command — parser MUST NOT special-case RPC.
  commands.registerCommand("call", &CoreFeature::cmdCall, "namespaced RPC ns.action");
}

void CoreFeature::contributeStatus(StatusBuilder& status) {
  int snr = 0;
  if (_host.readSnr(snr)) status.add("snr", snr);

  uint32_t heap = 0;
  if (_host.readHeapFree(heap)) status.add("heap", (unsigned long)heap);
}

bool CoreFeature::cmdPing(CommandContext& ctx) {
  ctx.reply->clear();
  ctx.reply->append("pong");
  return true;
}

bool CoreFeature::cmdStatus(CommandContext& ctx) {
  if (!g_core || !g_core->_host.engine) return false;
  StatusBuilder status;
  g_core->_host.engine->buildStatus(status);
  status.writeTo(*ctx.reply);
  return true;
}

bool CoreFeature::cmdDiscover(CommandContext& ctx) {
  if (!g_core || !g_core->_host.engine) return false;
  DiscoverBuilder discover;
  g_core->_host.engine->buildDiscover(discover);
  discover.writeTo(*ctx.reply);
  return true;
}

bool CoreFeature::cmdHelp(CommandContext& ctx) {
  if (!g_core || !g_core->_host.engine) {
    ctx.reply->clear();
    ctx.reply->append("ping status discovery help caps call");
    return true;
  }
  g_core->_host.engine->buildHelp(*ctx.reply);
  return true;
}

bool CoreFeature::cmdCaps(CommandContext& ctx) {
  if (!g_core || !g_core->_host.engine) {
    ctx.reply->clear();
    return true;
  }
  g_core->_host.engine->buildCaps(*ctx.reply);
  return true;
}

bool CoreFeature::cmdCall(CommandContext& ctx) {
  if (!g_core) {
    CallResult::err(*ctx.reply, "internal");
    return true;
  }

  if (ctx.request->argc < 1) {
    CallResult::err(*ctx.reply, "invalid_argument");
    return true;
  }

  const char* proc = ctx.request->args[0];
  if (!CallResult::isValidProc(proc)) {
    CallResult::err(*ctx.reply, "invalid_argument");
    CallResult::appendKv(*ctx.reply, "reason", "proc");
    return true;
  }

  if (g_core->_host.handleCall(proc, ctx)) {
    // Host wrote a CallResult line (or empty → dispatcher fills ok — avoid that).
    if (ctx.reply->len == 0) CallResult::ok(*ctx.reply);
    return true;
  }

  CallResult::err(*ctx.reply, "unknown_proc");
  return true;
}

}  // namespace mcrpc
