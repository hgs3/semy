// SPDX-License-Identifier: MIT
/*
 * Semy - a Semantic Versioning 2.0.0 processor.
 * See https://semver.org/.
 *
 * Copyright (c) 2025-2026 Henry G. Stratmann III
 * Copyright (c) 2025-2026 Semy Contributors
 *
 * This file is part of Semy, distributed under the MIT License.
 * For full terms see the included LICENSE file.
 */

#define UNIT_TESTING
#include "test_utils.h"
#include <stdlib.h>

static const char *const help_message =
    "Usage:\n"
    "\n"
    "  semver --compare <version1> <version2>\n"
    "  semver --decompose=<format> <version>\n"
    "  semver --sort <version>...\n"
    "  semver --validate <version>...\n"
    "\n"
    "Options:\n"
    "\n"
    "  -c <v1> <version2>\n"
    "  --compare <version1> <version2>\n"
    "       Compare semantic versions 'v1' and 'v2' and print -1, 0, 1 depending\n"
    "       on if v1 < v2, v1 = v2, v1 > v2 (respectively).\n"
    "\n"
    "  -d<format> <version>\n"
    "  -decompose=<format> <version>\n"
    "       Decompose a semantic version into its identifiers.\n"
    "\n"
    "  -s <versions>...\n"
    "  --sort <versions>...\n"
    "       Sort semantic versions and print them in ascending order on their\n"
    "       own line to stdout.\n"
    "\n"
    "  -V <versions>...\n"
    "  --validate <versions>...\n"
    "       Validate one or more semantic versions. If any semantic version is\n"
    "       invalid, the exit status will be 1.\n"
    "\n"
    "Exit status:\n"
    "  0  if OK,\n"
    "  1  if one or more semantic versions are malformed,\n"
    "  2  if the program arguments are incorrect,\n"
    "  3  if a general error occurred while processing the input.\n"
    "\n"
    "This program is distributed under the MIT License.\n";

static const struct CommandLineTestCase test_cases[] = {
    {
        {"semy", "-h"},
        0,
        help_message,
        "",
    },
    {
        {"semy", "--help"},
        0,
        help_message,
        "",
    },
};

TEST(semy, cli, .iterations=COUNT_OF(test_cases))
{
    run_cli_test(&test_cases[TEST_ITERATION]);
}
