get_filename_component(concurrentqueue_ROOT ${BASE_DIR}/output ABSOLUTE)

message("concurrentqueue_ROOT ${concurrentqueue_ROOT}")

find_package(concurrentqueue)

include(CMakePrintHelpers)
cmake_print_variables(concurrentqueue_FOUND)

function(concurrentqueue_deps Target)
    target_link_libraries(${Target} PRIVATE concurrentqueue::concurrentqueue)
endfunction(concurrentqueue_deps Target)