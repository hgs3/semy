#!/bin/bash

# This script runs Clang sanitizers (UBSAN, ASAN, and MSAN).
# You must run this script from this directory.

# Verify Clang is installed.
if ! command -v clang &> /dev/null; then
    echo "clang could not be found; please install it to gather code coverage"
    exit 1
fi
export CC=clang

# Undefined behavior sanitizer.
cmake .. -B ubsan-build -DSEMY_BUILD_TESTS=ON -DSEMY_UNDEFINED_BEHAVIOR_SANITIZER=ON
cmake --build ubsan-build
ctest --test-dir ubsan-build

# Address sanitizer.
cmake .. -B asan-build -DSEMY_BUILD_TESTS=ON -DSEMY_ADDRESS_SANITIZER=ON
cmake --build asan-build
ctest --test-dir asan-build

# Memory sanitizer.
cmake .. -B msan-build -DSEMY_BUILD_TESTS=ON -DSEMY_MEMORY_SANITIZER=ON
cmake --build msan-build
ctest --test-dir msan-build
