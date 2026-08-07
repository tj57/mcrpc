/**
 * Home Assistant–style peer: parse inbound events/responses and build requests.
 * No HA SDK — demonstrates the integration surface only.
 */
#include <mcrpc/mcrpc.h>
#include <cstdio>
#include <cstring>

using namespace mcrpc;

static void handleInbound(const char* text) {
  const char* line = Parser::stripSenderPrefix(text);
  if (std::strncmp(line, "event ", 6) == 0) {
    std::printf("HA_EVENT %s\n", line + 6);
    return;
  }
  if (line[0] == '#') {
    std::printf("HA_RESPONSE %s\n", line);
    return;
  }
  Request r;
  if (Parser::parse(line, r) == ParseResult::Ok) {
    std::printf("HA_SEEN_REQUEST cmd=%s\n", r.command);
  } else {
    std::printf("HA_DATA %s\n", line);
  }
}

int main() {
  ReplyBuffer out;
  OutboundBuilder::request(out, "tracker", "gps", true, 42);
  std::printf("OUT %s\n", out.data);

  handleInbound("tracker: event button.pressed count=1");
  handleInbound("tracker: #42 gps lat=1.0 lon=2.0");
  // Discover-style lines are data for HA (key=value), not commands
  const char* disc = "tracker id=aabbccdd fw=1 v=1.2 tag=tracker up=42s";
  if (std::strstr(disc, "v="))
    std::printf("HA_DISCOVER %s\n", disc);
  else
    handleInbound(disc);
  return 0;
}
