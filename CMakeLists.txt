cmake_minimum_required(VERSION 3.12)

project(Monitor)
set(CMAKE_CXX_STANDARD 14)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE} version ${CMAKE_C_COMPILER} ${CMAKE_CXX_COMPILER}")

# 设置基础变量
SET(BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/output)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

message("get DOWNLOAD_THIRDPARTY ${DOWNLOAD_THIRDPARTY}")
if(DOWNLOAD_THIRDPARTY)
    add_subdirectory(deps)
else(DOWNLOAD_THIRDPARTY)
    # 设置cmake module path路径便于find_package

    list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")

    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer ")

    set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address  -lasan -lstdc++ ")

    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address  -lasan -lstdc++ ")

    # 链接interface 便于开发
    INCLUDE(cmake/utils/intf.cmake)
    INCLUDE(cmake/utils/gtest.cmake)
    INCLUDE(cmake/utils/spdlog.cmake)
    INCLUDE(cmake/utils/yyjson.cmake)
    INCLUDE(cmake/utils/concurrentqueue.cmake)
    INCLUDE(cmake/utils/libuuid.cmake)

    set(MONITOR_MAJOR_VERSION 0)
    set(MONITOR_MINOR_VERSION 0)
    set(MONITOR_PATCH_VERSION 1)
    set(MONITOR_VERSION
        ${MONITOR_MAJOR_VERSION}.${MONITOR_MINOR_VERSION}.${MONITOR_PATCH_VERSION})
    set(MONITOR_VERSION_STRING "${MONITOR_VERSION}")

    option(MONITOR_BUILD_TESTS "Build Monitor's unit tests" ON)
    option(MONITOR_BUILD_BENCHMARKS "Build Monitor's benchmarks" ON)
    option(MONITOR_INSTALL "Install Monitor's header and library" ON)

    add_subdirectory(src)

    include(CTest)
    enable_testing()
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)
    message(STATUS "test")

    add_subdirectory(test)
endif(DOWNLOAD_THIRDPARTY)