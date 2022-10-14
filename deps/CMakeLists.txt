cmake_minimum_required(VERSION 3.12)

project(deps)

set(CMAKE_CXX_STANDARD 11)

SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../output)
SET(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty)
message("deps install ${CMAKE_INSTALL_PREFIX}")

INCLUDE(cmake/gtest.cmake)
INCLUDE(cmake/spdlog.cmake)
INCLUDE(cmake/leveldb.cmake)
INCLUDE(cmake/yyjson.cmake)
INCLUDE(cmake/asio.cmake)
INCLUDE(cmake/protobuf.cmake)

# INCLUDE(cmake/grpc.cmake)