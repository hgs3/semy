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

#include "semy.h"
#include <audition.h>

#define COUNT_OF(ARRAY) (sizeof(ARRAY)/sizeof((ARRAY)[0]))

#define LESS_THAN -1
#define EQUAL_TO 0
#define GREATER_THAN 1

static const struct TestCase {
    const char *LHS;
    const char *RHS;
    int32_t compare_result;
} test_cases[] = {
    // Core version.
    {"0.0.0", "0.0.0", EQUAL_TO},
    {"0.0.1", "0.0.1", EQUAL_TO},
    {"0.1.0", "0.1.0", EQUAL_TO},
    {"1.0.0", "1.0.0", EQUAL_TO},
    {"0.0.0", "0.0.1", LESS_THAN},
    {"0.0.0", "0.1.0", LESS_THAN},
    {"0.0.0", "1.0.0", LESS_THAN},
    {"0.0.1", "0.0.0", GREATER_THAN},
    {"0.1.0", "0.0.0", GREATER_THAN},
    {"1.0.0", "2.0.0", LESS_THAN},
    {"2.0.0", "2.1.0", LESS_THAN},
    {"2.1.0", "2.1.1", LESS_THAN},
    {"1.0.0", "0.0.0", GREATER_THAN},
    {"1.2.3", "1.1.3", GREATER_THAN},
    {"1.2.3", "1.3.3", LESS_THAN},
    {"1.2.3", "1.2.2", GREATER_THAN},
    {"1.2.3", "1.3.4", LESS_THAN},
    {"10.0.0", "1.0.0", GREATER_THAN},
    {"0.1.0", "0.10.0", LESS_THAN},
    {"0.0.1", "0.0.10", LESS_THAN},
    {"0.0.10", "0.0.0", GREATER_THAN},
    {"123.456.789", "123.456.789", EQUAL_TO},
    {"123.456.789", "1230.456.789", LESS_THAN},
    {"2147483647.2147483647.2147483647", "2147483647.2147483647.2147483647", EQUAL_TO},
    {"2147483647.0.0", "2147483646.0.0", GREATER_THAN},
    {"2147483647.2147483646.1", "2147483647.2147483647.1", LESS_THAN},

    // Pre-release version.
    {"1.0.0-alpha", "1.0.0-alpha", EQUAL_TO},
    {"1.0.0-alpha", "1.0.0-alpha.beta", LESS_THAN},
    {"1.0.0-alpha", "1.0.0-beta", LESS_THAN},
    {"1.0.0-beta", "1.0.0-alpha", GREATER_THAN},
    {"1.0.0-alpha", "1.0.0-alpha.1", LESS_THAN},
    {"1.0.0-alpha.1", "1.0.0-alpha", GREATER_THAN},
    {"1.0.0-alpha.1", "1.0.0-alpha.beta", LESS_THAN},
    {"1.0.0-alpha.beta", "1.0.0-alpha.1", GREATER_THAN},
    {"1.0.0-alpha.beta", "1.0.0-beta", LESS_THAN},
    {"1.0.0-beta", "1.0.0-alpha.beta", GREATER_THAN},
    {"1.0.0-alpha.beta", "1.0.0-beta.2", LESS_THAN},
    {"1.0.0-beta.2", "1.0.0-alpha.beta", GREATER_THAN},
    {"1.0.0-beta.2", "1.0.0-beta.11", LESS_THAN},
    {"1.0.0-beta.11", "1.0.0-beta.2", GREATER_THAN},
    {"1.0.0-rc.1", "1.0.0", LESS_THAN},
    {"1.2.3-rc.2", "1.2.3-rc.10", LESS_THAN},
    {"1.0.0", "1.0.0-rc.1", GREATER_THAN},
    {"1.0.0-alpha", "1.0.0", LESS_THAN},
    {"1.0.0", "1.0.0-alpha", GREATER_THAN},
    {"1.0.0-alpha", "1.0.1-alpha", LESS_THAN},
    {"1.0.0", "1.0.1-alpha", LESS_THAN},
    {"2.0.0-beta", "1.9.9", GREATER_THAN},
    {"1.0.0-9", "1.0.0-8", GREATER_THAN},
    {"1.0.0-123", "1.0.0-123", EQUAL_TO},
    {"1.0.0-123", "1.0.0-124", LESS_THAN},
    {"1.0.0-9", "1.0.0-a", LESS_THAN},
    {"1.0.0-123", "1.0.0-abc", LESS_THAN},
    {"1.0.0-9", "1.0.0-A", LESS_THAN},
    {"1.0.0-999999999", "1.0.0-a", LESS_THAN},
    {"1.0.0-alpha.10", "1.0.0-alpha.10b", LESS_THAN},
    {"1.0.0-rc.1", "1.0.0-rc.alpha", LESS_THAN},
    {"1.0.0-alpha", "1.0.0-alph", GREATER_THAN},
    {"1.0.0-abc", "1.0.0-abd", LESS_THAN},
    {"1.0.0-beta", "1.0.0-betadog", LESS_THAN},
    {"1.0.0-rc1", "1.0.0-rc2", LESS_THAN},
    {"1.0.0--", "1.0.0---", LESS_THAN},
    {"1.0.0-A", "1.0.0-a", LESS_THAN},
    {"1.0.0-alpha", "1.0.0-ALPHA", GREATER_THAN},
    {"1.0.0-B", "1.0.0-a", LESS_THAN},
    {"1.0.0-alpha.1.2", "1.0.0-alpha.1.2.3", LESS_THAN},
    {"1.2.3-1.2.3", "1.2.3-1.2.3.1", LESS_THAN},
    {"1.0.0--1", "1.0.0-1", GREATER_THAN},
    {"1.0.0--beta", "1.0.0-beta", LESS_THAN},
    {"1.0.0-rc.1.alpha", "1.0.0-rc.1.2", GREATER_THAN},

    // Build identifier (has no impact on precedence).
    {"1.0.0+0", "1.0.0+1", EQUAL_TO},
    {"1.0.0+alpha", "1.0.0+beta", EQUAL_TO},
    {"1.0.0-alpha+001", "1.0.0-alpha", EQUAL_TO},
    {"1.0.0-alpha+123", "1.0.0-alpha+456", EQUAL_TO},
    {"1.0.0-alpha.1+build", "1.0.0-alpha.1", EQUAL_TO},
    {"1.0.0+20130313144700", "1.0.0+20120313144700", EQUAL_TO},
};

TEST(semver, parse, .iterations=COUNT_OF(test_cases))
{
    const struct TestCase *test_case = &test_cases[TEST_ITERATION];
    semy_t v1 = {0};
    semy_t v2 = {0};
    int32_t result = -99;
    semy_error_t err;

    err = semy_init(&v1, sizeof(v1), test_case->LHS);
    ASSERT_EQ((int)err, SEMY_NO_ERROR);

    err = semy_init(&v2, sizeof(v2), test_case->RHS);
    ASSERT_EQ((int)err, SEMY_NO_ERROR);

    err = semy_compare(&v1, &v2, &result);
    ASSERT_EQ((int)err, SEMY_NO_ERROR);

    ASSERT_EQ(test_case->compare_result, result, "unexpected compare result for '%s' and '%s'", test_case->LHS, test_case->RHS);
}

TEST(semver, null_arguments)
{
    semy_error_t err = SEMY_NO_ERROR;
    int32_t result = 0;
    semy_t semver = {0};
    
    err = semy_init(&semver, sizeof(semver), "1.0.0");
    ASSERT_EQ((int)SEMY_NO_ERROR, err);

    err = semy_compare(&semver, NULL, &result);
    ASSERT_EQ((int)SEMY_INVALID_OPERATION, err);

    err = semy_compare(NULL, &semver, &result);
    ASSERT_EQ((int)SEMY_INVALID_OPERATION, err);

    err = semy_compare(&semver, &semver, NULL);
    ASSERT_EQ((int)SEMY_INVALID_OPERATION, err);
}
