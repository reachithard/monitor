configure_file (
    "${BASE_DIR}/src/monitor_version.h.in"
    "${BASE_DIR}/src/export/monitor_version.h"
)

add_library(mon_config ./config/mon_config.cpp)
target_sources(mon_config PRIVATE 
./config/mon_json.cpp
)
target_link_libraries(mon_config PRIVATE intf)
yyjson_deps(mon_config)