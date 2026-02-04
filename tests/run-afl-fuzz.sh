#!/bin/bash

# This script runs American Fuzzy Lop (ALF) fuzzer against the Semy command-line interface.
# You must run this script manually from this directory.

export AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1
export AFL_SKIP_CPUFREQ=1
export CC=afl-cc

cmake .. -B fuzz-build -DSEMY_BUILD_TESTS=ON
cmake --build fuzz-build
afl-fuzz -t 250 -i ./corpus -o . -S fuzz-state -- ./fuzz-build/semy -V @@
