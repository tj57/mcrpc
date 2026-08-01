/**
 * libFuzzer harness for Parser (optional; Clang -DMCRPC_BUILD_FUZZERS=ON).
 *
 * Build:
 *   cmake -S lib/mcrpc -B build/mcrpc -DMCRPC_BUILD_FUZZERS=ON -DCMAKE_CXX_COMPILER=clang++
 *   cmake --build build/mcrpc --target mcrpc_fuzz_parser
 *   ./build/mcrpc/mcrpc_fuzz_parser -max_total_time=60
 *
 * AFL++ (alternative): compile with afl-clang-fast++ and feed stdin lines.
 * Keep the harness free of Arduino/MeshCore so it stays portable.
 */
#include <mcrpc/Parser.h>
#include <stdint.h>
#include <stddef.h>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size > 512) size = 512;
  std::string s(reinterpret_cast<const char*>(data), size);
  mcrpc::Request req;
  (void)mcrpc::Parser::parse(s.c_str(), req);
  (void)mcrpc::Parser::stripSenderPrefix(s.c_str());
  return 0;
}
