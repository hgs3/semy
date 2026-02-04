#!/usr/bin/env bash

# This script gathers code coverage metrics and generates an HTML report.
# You must run this script from this directory.

# Verify LCOV is installed.
if ! command -v lcov &> /dev/null; then
    echo "lcov could not be found; please install it to gather code coverage"
    exit 1
fi

# Verify GCC is installed.
if ! command -v gcc &> /dev/null; then
    echo "gcc could not be found; please install it to gather code coverage"
    exit 1
fi
export CC=gcc

# Clear any existing coverage report before generating a new one.
# Otherwise you might get a stale report.
rm -f coverage.info

# Build the project and execute the tests to generate a coverage report.
cmake .. -B coverage-build -DSEMY_BUILD_TESTS=ON -DSEMY_CODE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build coverage-build
ctest --test-dir coverage-build

# Capture code coverage data.
lcov --capture --directory . --output-file coverage.info

# Remove third-party libraries.
lcov --remove coverage.info '/usr/*' -o coverage.info

# Remove unit test coverage data.
lcov --remove coverage.info "$1/tests/*" -o coverage.info

# Generate an HTML code coverage report.
genhtml coverage.info --output-directory coverage-report

# Get coverage percentage.
COVERAGE=$(lcov --summary coverage.info | grep 'lines' | awk '{print $2}' | sed 's/%//')

# Bash does not support comparing floating-point numbers;
# remove the fractional part from the percentage.
COVERAGE=${COVERAGE%.*}

# Minimum coverage threshold.
THRESHOLD=100

# Check if coverage is above threshold.
if [ "$COVERAGE" -lt "$THRESHOLD" ]; then
    echo "Code coverage is below the threshold: $COVERAGE% < $THRESHOLD%"
    exit 1
fi
