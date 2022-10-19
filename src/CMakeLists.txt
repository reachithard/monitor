configure_file (
    "${BASE_DIR}/src/monitor_version.h.in"
    "${BASE_DIR}/src/export/monitor_version.h"
)

add_library(mon_config ./config/mon_config.cpp)
target_sources(mon_config PRIVATE  
./config/mon_json.cpp
./config/mon_config_utils.cpp
)
target_link_libraries(mon_config PRIVATE intf)
yyjson_deps(mon_config)

add_library(mem_core ./core/mon_process.cpp)
target_sources(mem_core PRIVATE  
                    ./core/mon_cache.cpp
                    ./core/mon_confile.cpp
                    ./core/monitor_cxx.cpp
                    ./core/mon_parse.cpp
                    ./core/mon_plugin.cpp
                    ./core/mon_process.cpp
                    ./core/mon_timer.cpp
)
target_link_libraries(mem_core PRIVATE intf mon_config)