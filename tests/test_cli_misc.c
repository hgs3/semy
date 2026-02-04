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

static const struct CommandLineTestCase test_cases[] = {
    {
        {"semy"},
        0,
        "Usage:\n"
        "\n"
        "  semver --compare <version1> <version2>\n"
        "  semver --decompose=<format> <version>\n"
        "  semver --sort <version>...\n"
        "  semver --validate <version>...\n"
        "\n"
        "Run 'semver --help' for more information.\n",
        "",
    },
    {
        {"semy", "-x"},
        2,
        "",
        "error: invalid argument '-x'\n",
    },
};

TEST(semy, cli, .iterations=COUNT_OF(test_cases))
{
    run_cli_test(&test_cases[TEST_ITERATION]);
}
