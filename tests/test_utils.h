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

#include <audition.h>

#define COUNT_OF(ARRAY) (sizeof(ARRAY)/sizeof((ARRAY)[0]))

struct CommandLineTestCase
{
    const char *argv[16];
    int exit_code;
    const char *out;
    const char *err;
};

void run_cli_test(const struct CommandLineTestCase *test_case);
