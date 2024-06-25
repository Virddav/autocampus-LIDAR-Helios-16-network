#!/bin/bash

cd $(dirname "$0")
rm -rf ../aoc_dependencies/build/
cmake -S ../aoc_dependencies/ -B ../aoc_dependencies/build -DCMAKE_TOOLCHAIN_FILE=../aoc_dependencies/arm64-toolchain.cmake
cd ../aoc_dependencies/build
sudo make install
