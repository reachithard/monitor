configure_file (
    "${BASE_DIR}/src/monitor_version.h.in"
    "${BASE_DIR}/src/export/monitor_version.h"
)

add_library(mconfig ./config/mon_config.cpp)
target_sources(mconfig PRIVATE  
./config/mon_json.cpp
./config/mon_config_utils.cpp
)
target_link_libraries(mconfig PRIVATE intf)
yyjson_deps(mconfig)

add_library(mcore ./core/mon_core.cpp)
target_sources(mcore PRIVATE  
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
target_link_libraries(mcore PRIVATE intf mconfig dl)
target_link_options(mcore PRIVATE "-Wl,--export-dynamic")
concurrentqueue_deps(mcore)

# 插件
add_library(test_plguin SHARED ./plugin/test_plugin.cpp)
target_link_libraries(test_plguin PRIVATE intf)
SET_TARGET_PROPERTIES (test_plguin PROPERTIES PREFIX "")
