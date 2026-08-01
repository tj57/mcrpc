#include <mcrpc/mcrpc.h>
#include <cstdio>

int main() {
  mcrpc::ReplyBuffer out;
  mcrpc::OutboundBuilder::request(out, "tracker", "gps", true, 18);
  std::printf("%s\n", out.data);
  mcrpc::OutboundBuilder::event(out, "button_pressed", "count=1");
  std::printf("%s\n", out.data);
  return 0;
}
