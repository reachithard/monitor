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
                ./core/mon_cache.cpp
                ./core/mon_cache.h
                ./core/mon_confile.cpp
                ./core/mon_confile.h
                ./core/mon_core.h
                ./core/mon_module.cpp
                ./core/mon_module.h
                ./core/mon_parse.cpp
                ./core/mon_parse.h
                ./core/mon_plugin.cpp
                ./core/mon_timer.cpp
                ./core/mon_timer.h
                ./core/mon_work.cpp
                ./core/mon_work.h
)
target_link_libraries(mcore PRIVATE intf mconfig dl)
target_link_options(mcore PRIVATE "-Wl,--export-dynamic")

# 插件
add_library(test_plguin SHARED ./plugin/test_plugin.cpp)
target_link_libraries(test_plguin PRIVATE intf)
SET_TARGET_PROPERTIES (test_plguin PROPERTIES PREFIX "")
