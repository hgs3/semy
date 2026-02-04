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
        {"semy", "--sort", "2.0.0", "2.0.0-rc.1", "2.0.0-beta.11", "2.0.0-beta.2", "2.0.0-beta", "2.0.0-alpha.beta", "2.0.0-alpha.1", "2.0.0-alpha"},
        0,
        "2.0.0-alpha\n"
        "2.0.0-alpha.1\n"
        "2.0.0-alpha.beta\n"
        "2.0.0-beta\n"
        "2.0.0-beta.2\n"
        "2.0.0-beta.11\n"
        "2.0.0-rc.1\n"
        "2.0.0\n",
        "",
    },
    {
        {"semy", "-s"},
        0,
        "",
        "",
    },
    {
        {"semy", "-s", "1.0.0"},
        0,
        "1.0.0\n",
        "",
    },
    {
        {"semy", "-s", "2.0.0", "2.0.0beta", "2.0.1"},
        1,
        "",
        "error: invalid semantic version\n",
    },
};

TEST(semy, cli, .iterations=COUNT_OF(test_cases))
{
    run_cli_test(&test_cases[TEST_ITERATION]);
}

