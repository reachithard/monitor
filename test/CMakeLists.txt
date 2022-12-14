configure_file (
    "${BASE_DIR}/test/resources/test_runtime.h.in"
    "${BASE_DIR}/test/test_runtime.h"
)

# 因为用了动态库 dlopen 防止undefined reference
do_unit_test(monitor_tests gtest_inlet.cpp)
target_sources(monitor_tests
  PRIVATE
    "${BASE_DIR}/test/test_libuuid.cpp"
    "${BASE_DIR}/src/config/mon_config_test.cc"
    "${BASE_DIR}/src/config/mon_json_test.cc"
    "${BASE_DIR}/src/core/mon_confile_test.cc"
    "${BASE_DIR}/src/core/mon_core_test.cc"
    "${BASE_DIR}/src/core/mon_timer_test.cc"
    ${BASE_DIR}/src/core/mon_actor.cpp
    ${BASE_DIR}/src/core/mon_actor.h
    ${BASE_DIR}/src/core/mon_cache.cpp
    ${BASE_DIR}/src/core/mon_cache.h
    ${BASE_DIR}/src/core/mon_confile.cpp
    ${BASE_DIR}/src/core/mon_confile.h
    ${BASE_DIR}/src/core/mon_core.cpp
    ${BASE_DIR}/src/core/mon_core.h
    ${BASE_DIR}/src/core/monitor_cxx.cpp
    ${BASE_DIR}/src/core/monitor_cxx.h
    ${BASE_DIR}/src/core/mon_module.cpp
    ${BASE_DIR}/src/core/mon_module.h
    ${BASE_DIR}/src/core/mon_parse.cpp
    ${BASE_DIR}/src/core/mon_parse.h
    ${BASE_DIR}/src/core/mon_plugin.cpp
    ${BASE_DIR}/src/core/mon_timer.cpp
    ${BASE_DIR}/src/core/mon_timer.h
    ${BASE_DIR}/src/core/mon_work.cpp
    ${BASE_DIR}/src/core/mon_work.h
)
target_link_libraries(monitor_tests PRIVATE mconfig dl)
yyjson_deps(monitor_tests)
uuid_deps(monitor_tests)
concurrentqueue_deps(monitor_tests)
# 该选项必须要加 加了才能够正常的调用主进程的函数
target_link_options(monitor_tests PRIVATE "-Wl,--export-dynamic")