#include <mcrpc/mcrpc.h>
#include <cstdio>

int main() {
  mcrpc::Request r;
  const char* line = "tracker#7 gps";
  if (mcrpc::Parser::parse(line, r) != mcrpc::ParseResult::Ok) return 1;
  std::printf("command=%s id=%lu\n", r.command, (unsigned long)r.request_id);
  return 0;
}
