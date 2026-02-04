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
        {"semy", "-v"},
        0,
        "1.0.0-alpha\n",
        "",
    },
    {
        {"semy", "--version"},
        0,
        "1.0.0-alpha\n",
        "",
    },
};

TEST(semy, cli, .iterations=COUNT_OF(test_cases))
{
    run_cli_test(&test_cases[TEST_ITERATION]);
}
