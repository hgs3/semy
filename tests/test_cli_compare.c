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
        {"semy", "-c", "2.0.0", "2.0.0"},
        0,
        "0\n",
        "",
    },
    {
        {"semy", "-c", "1.0.0-beta", "1.0.0-alpha"},
        0,
        "1\n",
        "",
    },
    {
        {"semy", "-c", "0.9.1", "0.10.0"},
        0,
        "-1\n",
        "",
    },
    {
        {"semy", "--compare", "2.0.0", "2.0.0"},
        0,
        "0\n",
        "",
    },
    {
        {"semy", "--compare", "1.0.0-beta", "1.0.0-alpha"},
        0,
        "1\n",
        "",
    },
    {
        {"semy", "--compare", "0.9.1", "0.10.0"},
        0,
        "-1\n",
        "",
    },
    {
        {"semy", "-c", "1.0.0", "1.0.0", "1.0.0"},
        2,
        "",
        "error: expected exactly two version strings\n",
    },
    {
        {"semy", "-c", "3.4.1-alpha"},
        2,
        "",
        "error: expected exactly two version strings\n",
    },
    {
        {"semy", "-c", "1.0.0", "1.0.0b"},
        1,
        "",
        "error: invalid semantic version\n",
    },
};

TEST(semy, cli, .iterations=COUNT_OF(test_cases))
{
    run_cli_test(&test_cases[TEST_ITERATION]);
}
