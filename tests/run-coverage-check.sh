#!/usr/bin/env bash

# This script gathers code coverage metrics and generates an HTML report.
# You must run this script from this directory.

# Verify gcovr is installed.
if ! command -v gcovr &> /dev/null; then
    echo "gcovr could not be found; please install it to gather code coverage"
    exit 1
fi

# Verify gcc is installed.
if ! command -v gcc &> /dev/null; then
    echo "gcc could not be found; please install it to gather code coverage"
    exit 1
fi

# Build the project and execute the tests to generate a coverage report.
CC=gcc cmake .. -B coverage-build -DSEMY_BUILD_TESTS=ON -DSEMY_CODE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build coverage-build
ctest --test-dir coverage-build

# Clear and create the output directory.
rm -rf coverage-report
mkdir coverage-report

# Capture code coverage data.
gcovr -r .. --exclude $(pwd) --html --html-details -o coverage-report/coverage.html --fail-under-line 100
