configure_file (
    "${BASE_DIR}/test/resources/test_runtime.h.in"
    "${BASE_DIR}/test/test_runtime.h"
)

do_unit_test(monitor_tests gtest_inlet.cpp)
target_sources(monitor_tests
  PRIVATE
    "${BASE_DIR}/src/config/mon_config_test.cc"
    "${BASE_DIR}/src/config/mon_json_test.cc"
    "${BASE_DIR}/src/core/mon_confile_test.cc"
)
target_link_libraries(monitor_tests PRIVATE mon_config mem_core)
yyjson_deps(monitor_tests)