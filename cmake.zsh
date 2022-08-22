#!/bin/zsh -e

local __dirname=${0:h}
cd $__dirname

if [ ! -e vcpkg/vcpkg ]; then
  vcpkg/bootstrap-vcpkg.sh -disableMetrics
fi

vcpkg/vcpkg install pcapplusplus

cmake -B build -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
