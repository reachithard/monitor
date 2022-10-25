#!/usr/bin/env bash

set -eux

git submodule add git@github.com:google/googletest.git ./thirdparty/gtest
git submodule add git@github.com:gabime/spdlog.git ./thirdparty/spdlog
git submodule add git@github.com:google/leveldb.git ./thirdparty/leveldb
git submodule add git@github.com:ibireme/yyjson.git ./thirdparty/yyjson
git submodule add git@github.com:chriskohlhoff/asio.git ./thirdparty/asio
git submodule add git@github.com:protocolbuffers/protobuf.git ./thirdparty/protobuf
git submodule add git@github.com:grpc/grpc.git ./thirdparty/grpc
git submodule add git@github.com:cameron314/concurrentqueue.git ./thirdparty/concurrentqueue