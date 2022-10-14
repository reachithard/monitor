find_package(GTest REQUIRED)
include(GoogleTest)

function(do_unit_test TARGET SRC)
    add_executable(${TARGET} ${SRC})
    target_link_libraries(${TARGET} PRIVATE GTest::gtest_main GTest::gmock intf)
    gtest_discover_tests(${TARGET})
    add_dependencies(check ${TARGET})
endfunction(do_unit_test)