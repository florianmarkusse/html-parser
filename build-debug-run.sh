#!/bin/bash
set -e

cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Debug && cmake --build build/
./run-executable.sh build html-parser-debug
