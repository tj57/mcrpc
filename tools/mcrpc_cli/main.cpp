/**
 * mcrpc — first-class CLI (public API only).
 *
 *   mcrpc parse|decode "<line>"
 *   mcrpc validate "<line>"
 *   mcrpc build request <target> [#id] <command> [args...]
 *   mcrpc build event <name> [kv...]
 *   mcrpc encode ...          (alias: build request)
 *   mcrpc discover [name]     (sample discover line)
 *   mcrpc ping [target]       (build a ping request)
 *   mcrpc version
 */
#include <mcrpc/mcrpc.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

using namespace mcrpc;

static void usage() {
  std::fprintf(stderr,
               "mcrpc %s (protocol %s)\n"
               "Commands:\n"
               "  parse|decode \"<line>\"\n"
               "  validate \"<line>\"\n"
               "  build request <target> [#id] <command> [args...]\n"
               "  build event <name> [key=value...]\n"
               "  encode <target> [#id] <command> [args...]\n"
               "  discover [node_name]\n"
               "  ping [target]\n"
               "  version\n",
               sdkVersionString(), protocolVersionString());
}

static int cmdParse(const char* line) {
  Request r;
  ParseResult pr = Parser::parse(line, r);
  if (pr != ParseResult::Ok) {
    std::printf("parse_error=%d\n", (int)pr);
    return 1;
  }
  const char* kind = "named";
  if (r.address_kind == AddressKind::All) kind = "all";
  else if (r.address_kind == AddressKind::Self) kind = "self";
  else if (r.address_kind == AddressKind::Group) kind = "group";
  std::printf("ok\naddress_kind=%s\ntarget=%s\n", kind, r.target);
  if (r.has_request_id) std::printf("request_id=%lu\n", (unsigned long)r.request_id);
  std::printf("command=%s\n", r.command);
  for (uint8_t i = 0; i < r.argc; i++) std::printf("arg%u=%s\n", (unsigned)i, r.args[i]);
  return 0;
}

static int cmdValidate(const char* line) {
  Request r;
  if (Parser::parse(line, r) == ParseResult::Ok) {
    std::printf("valid\n");
    return 0;
  }
  std::printf("invalid\n");
  return 1;
}

static int cmdBuildRequest(int argc, char** argv) {
  if (argc < 2) {
    usage();
    return 2;
  }
  char glued[96];
  std::snprintf(glued, sizeof(glued), "%s", argv[0]);
  const char* target = glued;
  bool has_id = false;
  uint32_t rid = 0;
  int idx = 1;
  char* hash = strchr(glued, '#');
  if (hash) {
    *hash = 0;
    has_id = true;
    rid = (uint32_t)strtoul(hash + 1, nullptr, 10);
  } else if (idx < argc && argv[idx][0] == '#') {
    has_id = true;
    rid = (uint32_t)strtoul(argv[idx] + 1, nullptr, 10);
    idx++;
  }
  if (idx >= argc) {
    usage();
    return 2;
  }
  const char* command = argv[idx++];
  ReplyBuffer out;
  OutboundBuilder::request(out, target, command, has_id, rid);
  for (; idx < argc; idx++) {
    out.appendChar(' ');
    out.append(argv[idx]);
  }
  std::printf("%s\n", out.data);
  return 0;
}

static int cmdBuildEvent(int argc, char** argv) {
  if (argc < 1) {
    usage();
    return 2;
  }
  std::string kv;
  for (int i = 1; i < argc; i++) {
    if (!kv.empty()) kv.push_back(' ');
    kv += argv[i];
  }
  ReplyBuffer out;
  OutboundBuilder::event(out, argv[0], kv.empty() ? nullptr : kv.c_str());
  std::printf("%s\n", out.data);
  return 0;
}

static int cmdDiscover(const char* name) {
  DiscoverBuilder d;
  d.setNodeName(name && name[0] ? name : "node");
  d.add("profile", "desktop");
  d.add("fw", "cli");
  d.add("protocol", protocolVersionString());
  d.add("sdk", sdkVersionString());
  ReplyBuffer out;
  d.writeTo(out);
  std::printf("%s\n", out.data);
  return 0;
}

static int cmdPing(const char* target) {
  ReplyBuffer out;
  OutboundBuilder::request(out, target && target[0] ? target : "all", "ping");
  std::printf("%s\n", out.data);
  return 0;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    usage();
    return 2;
  }
  const char* cmd = argv[1];
  if (std::strcmp(cmd, "version") == 0) {
    std::printf("library=%s\nsdk=%s\nprotocol=%s\n", libraryVersionString(),
                sdkVersionString(), protocolVersionString());
    return 0;
  }
  if (std::strcmp(cmd, "parse") == 0 || std::strcmp(cmd, "decode") == 0) {
    if (argc < 3) {
      usage();
      return 2;
    }
    return cmdParse(argv[2]);
  }
  if (std::strcmp(cmd, "validate") == 0) {
    if (argc < 3) {
      usage();
      return 2;
    }
    return cmdValidate(argv[2]);
  }
  if (std::strcmp(cmd, "discover") == 0) {
    return cmdDiscover(argc >= 3 ? argv[2] : "node");
  }
  if (std::strcmp(cmd, "ping") == 0) {
    return cmdPing(argc >= 3 ? argv[2] : "all");
  }
  if (std::strcmp(cmd, "build") == 0) {
    if (argc < 3) {
      usage();
      return 2;
    }
    if (std::strcmp(argv[2], "request") == 0) return cmdBuildRequest(argc - 3, argv + 3);
    if (std::strcmp(argv[2], "event") == 0) return cmdBuildEvent(argc - 3, argv + 3);
    usage();
    return 2;
  }
  if (std::strcmp(cmd, "encode") == 0) return cmdBuildRequest(argc - 2, argv + 2);
  usage();
  return 2;
}
