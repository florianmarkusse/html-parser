#!/bin/bash
set -e

source ./build.sh

EXECUTABLE="html-parser"
if [ "$RUN_TESTS" = true ]; then
	./run-executable.sh build "$EXECUTABLE"-tests-"$BUILD_MODE"
fi

if [ "$RUN_BENCHMARKS" = true ]; then
	./run-executable.sh build "$EXECUTABLE"-benchmarks-"$BUILD_MODE"
fi
