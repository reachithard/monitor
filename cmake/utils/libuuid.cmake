find_package(libuuid REQUIRED)

include(CMakePrintHelpers)
cmake_print_variables(libuuid_FOUND)
cmake_print_variables(libuuid_INCLUDE_DIR)
cmake_print_variables(libuuid_LIBRARY)

function(uuid_deps Target)
    target_link_libraries(${Target} PRIVATE libuuid::libuuid)
endfunction(uuid_deps Target)