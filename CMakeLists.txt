cmake_minimum_required(VERSION 3.16)
project(mcrpc VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(MCRPC_BUILD_TESTS "Build unit / compliance / golden / integration tests" ON)
option(MCRPC_BUILD_CLI "Build mcrpc CLI" ON)
option(MCRPC_BUILD_EXAMPLES "Build example programs" ON)
option(MCRPC_BUILD_FUZZERS "Build libFuzzer harness (Clang)" OFF)

add_library(mcrpc
  src/Parser.cpp
  src/Dispatcher.cpp
  src/CommandRegistry.cpp
  src/CapabilityRegistry.cpp
  src/EventBus.cpp
  src/FeatureManager.cpp
  src/McRpc.cpp
  src/Config.cpp
  src/features/core/CoreFeature.cpp
  src/features/battery/BatteryFeature.cpp
  src/features/button/ButtonFeature.cpp
  src/features/gps/GpsFeature.cpp
  src/features/relay/RelayFeature.cpp
  src/features/display/DisplayFeature.cpp
)
add_library(mcrpc::mcrpc ALIAS mcrpc)

target_include_directories(mcrpc
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_options(mcrpc PRIVATE -Wall -Wextra)

include(GNUInstallDirs)
install(DIRECTORY include/mcrpc DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS mcrpc
  EXPORT mcrpcTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(EXPORT mcrpcTargets
  FILE mcrpcTargets.cmake
  NAMESPACE mcrpc::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mcrpc
)
include(CMakePackageConfigHelpers)
configure_file(cmake/mcrpcConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/mcrpcConfig.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mcrpcConfig.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mcrpc)

if(MCRPC_BUILD_CLI)
  add_executable(mcrpc_cli tools/mcrpc_cli/main.cpp)
  target_link_libraries(mcrpc_cli PRIVATE mcrpc)
  set_target_properties(mcrpc_cli PROPERTIES OUTPUT_NAME mcrpc)
  install(TARGETS mcrpc_cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if(MCRPC_BUILD_EXAMPLES)
  add_subdirectory(examples/parser)
  add_subdirectory(examples/builder)
  add_subdirectory(examples/desktop)
  add_subdirectory(examples/firmware)
  add_subdirectory(examples/homeassistant)
endif()

if(MCRPC_BUILD_TESTS)
  enable_testing()
  add_executable(mcrpc_unit_tests tests/unit/test_mcrpc.cpp)
  target_link_libraries(mcrpc_unit_tests PRIVATE mcrpc)
  add_test(NAME mcrpc_unit COMMAND mcrpc_unit_tests)

  add_executable(mcrpc_compliance_tests tests/compliance/test_compliance.cpp)
  target_link_libraries(mcrpc_compliance_tests PRIVATE mcrpc)
  add_test(NAME mcrpc_compliance COMMAND mcrpc_compliance_tests)

  add_executable(mcrpc_golden_tests tests/golden/test_golden.cpp)
  target_link_libraries(mcrpc_golden_tests PRIVATE mcrpc)
  target_compile_definitions(mcrpc_golden_tests PRIVATE
    MCRPC_GOLDEN_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/golden/cases")
  add_test(NAME mcrpc_golden COMMAND mcrpc_golden_tests)

  add_executable(mcrpc_integration_tests tests/integration/test_integration.cpp)
  target_link_libraries(mcrpc_integration_tests PRIVATE mcrpc)
  add_test(NAME mcrpc_integration COMMAND mcrpc_integration_tests)
endif()

if(MCRPC_BUILD_FUZZERS AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_executable(mcrpc_fuzz_parser tests/fuzz/fuzz_parser.cpp)
  target_link_libraries(mcrpc_fuzz_parser PRIVATE mcrpc)
  target_compile_options(mcrpc_fuzz_parser PRIVATE -fsanitize=fuzzer,address)
  target_link_options(mcrpc_fuzz_parser PRIVATE -fsanitize=fuzzer,address)
endif()
