get_filename_component(yyjson_ROOT ${BASE_DIR}/output ABSOLUTE)

message("yyjson_ROOT ${yyjson_ROOT}")

find_package(yyjson)

include(CMakePrintHelpers)
cmake_print_variables(yyjson_FOUND)

function(yyjson_deps Target)
    target_link_libraries(${Target} PRIVATE yyjson::yyjson)
endfunction(yyjson_deps Target)