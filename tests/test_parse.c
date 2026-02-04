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
#include "test_utils.h"
#include <stdio.h>
#include <string.h>

static const struct TestCase {
    const char *string;
    semy_error_t status_code;
} test_cases[] = {
    // Basic core version test cases.
    {"0.0.0"},
    {"0.0.1"},
    {"0.1.1"},
    {"0.1.0"},
    {"1.0.0"},
    {"1.0.0"},
    {"1.1.1"},
    {"0.2.3"},
    {"4.5.6"},
    {"7.8.9"},
    {"1.9.0"},
    {"1.10.0"},
    {"1.11.0"},
    {"10.11.12"},
    {"987.321.456"},
    {"45.1.234"},
    {"01.1.1", SEMY_BAD_SYNTAX},
    {" 1.2.3", SEMY_BAD_SYNTAX},
    {"1.2.3 ", SEMY_BAD_SYNTAX},
    {"v1.2.3 ", SEMY_BAD_SYNTAX},
    {"1", SEMY_BAD_SYNTAX},
    {"1.2", SEMY_BAD_SYNTAX},
    {".2.3", SEMY_BAD_SYNTAX},
    {"1.2.3.4", SEMY_BAD_SYNTAX},
    {"-1.0.0", SEMY_BAD_SYNTAX},
    {"1.-2.3", SEMY_BAD_SYNTAX},
    {"1..0", SEMY_BAD_SYNTAX},
    {"1.0.", SEMY_BAD_SYNTAX},

    // Pushing the limits of the core version numbers.
    {"2147483647.0.0"},
    {"0.2147483647.0"},
    {"0.0.2147483647"},
    {"2147483648.0.0", SEMY_LIMITS_EXCEEDED},
    {"0.2147483648.0", SEMY_LIMITS_EXCEEDED},
    {"0.0.2147483648", SEMY_LIMITS_EXCEEDED},
    {"9223372036854775808.0.0", SEMY_LIMITS_EXCEEDED},

    // Pre-release identifier (alphanumeric).
    {"0.0.1--"},
    {"0.0.1-a"},
    {"0.0.1-z"},
    {"0.0.1-A"},
    {"0.0.1-Z"},
    {"1.0.0-0a"},
    {"1.0.0-00", SEMY_BAD_SYNTAX},
    {"1.2.3-0-"},
    {"0.0.1---"},
    {"0.0.1------"},
    {"0.0.1----0"},
    {"0.0.1---123"},
    {"0.0.1----a"},
    {"0.0.1---abc"},
    {"0.0.1---Z23---0--1-Xyz---"},
    {"1.0.0-", SEMY_BAD_SYNTAX},
    {"1.0.0-alpha"},
    {"1.0.0-beta"},
    {"1.0.0-beta-"},
    {"1.0.0-beta-"},
    {"1.0.0-BETA-"},
    {"1.0.0-zeta-gamma"},
    {"1.0.0---zeta---gamma---"},
    {"1.0.0-a.b.c"},
    {"1.0.0-x-y-z.--"},
    {"1.0.0-alpha.BETA.Gamma"},
    {"1.0.0-alpha..one", SEMY_BAD_SYNTAX},

    // Pre-release identifier (numeric).
    {"1.2.3-0"},
    {"1.2.3-01", SEMY_BAD_SYNTAX},
    {"1.2.3-012", SEMY_BAD_SYNTAX},
    {"1.2.3-1"},
    {"1.2.3-100"},
    {"1.2.3-12089"},
    {"1.0.0-0.3.7"},
    {"1.0.0-2147483647"},
    {"1.0.0-2147483648", SEMY_LIMITS_EXCEEDED},

    // Pre-release identifiers mixed numeric and alphanumeric.
    {"1.0.0-alpha.0"},
    {"1.0.0-alpha.1"},
    {"1.0.0-alpha.9"},
    {"1.0.0-alpha.10"},
    {"1.0.0-alpha.102030"},
    {"1.0.0-alpha.102030.beTa.0.gamma.91"},
    {"1.0.0-x.7.z.92"},
    {"1.0.0-beta.2"},
    {"1.0.0-beta.11"},
    {"1.0.0-rc.1"},
    {"1.2.3-rc.1-staging"},
    {"1.0.0-.", SEMY_BAD_SYNTAX},
    {"-alpha", SEMY_BAD_SYNTAX},

    // Build identifier.
    {"1.0.0+a"},
    {"1.0.0+ab"},
    {"1.0.0+0"},
    {"1.0.0+01"},
    {"1.0.0+0123"},
    {"1.0.0+-"},
    {"1.0.0+---"},
    {"1.0.0+---aBc---"},
    {"1.0.0+a-0-b-9-c"},
    {"1.0.0+20130313144700"},
    {"1.0.0+21AF26D3----117B344092BD"},
    {"1.0.0+", SEMY_BAD_SYNTAX},
    {"1.0.0+build..2", SEMY_BAD_SYNTAX},
    {"-alpha", SEMY_BAD_SYNTAX},
    {"+build", SEMY_BAD_SYNTAX},
    {"1.2.3+builD+CombineD", SEMY_BAD_SYNTAX},

    // Full integration.
    {"1.0.0-alpha+001"},
    {"1.0.0-beta+exp.sha.5114f85"},
    {"1.0.0-alpha_beta", SEMY_BAD_SYNTAX},
    {"1.0.0+build.!", SEMY_BAD_SYNTAX},
    {u8"1.0.0-🚀", SEMY_BAD_SYNTAX},
    {"1.0.0-+", SEMY_BAD_SYNTAX},
    {"1.2.3a", SEMY_BAD_SYNTAX},
    {"1.2.3-alpha!@#", SEMY_BAD_SYNTAX},
    {"1.2.3.foo", SEMY_BAD_SYNTAX},
    {"1.0.0-some-really-really-REALLY-l0ng-1dentifier-that-just-keeps-going-and-going-and-going-and-going-and-going-and-going-and-going-and-going-and-going-and-going-and-going-and-going"},
    {"1.0.0-some.really.really.really.l0ng.1dentifier.that.just.keeps.going.and.going.and.going+but-turns-into-build-meta-data-and-keeps-going-and-going-and-going"},
};

TEST(semver, parse, .iterations=COUNT_OF(test_cases))
{
    const struct TestCase *test_case = &test_cases[TEST_ITERATION];
    const char *input = test_case->string;

    semy_t semver = {0};
    const semy_error_t err = semy_parse(&semver, sizeof(semver), input);
    ASSERT_EQ((int)err, test_case->status_code, "unexpected error code parsing: %s", input);

    // If parsing is expected to fail, then don't extract the data.
    if (test_case->status_code != SEMY_NO_ERROR)
    {
        return;
    }

    char output[4096] = {0};
    sprintf(output, "%d.%d.%d", semy_get_major(&semver), semy_get_minor(&semver), semy_get_patch(&semver));

    int32_t count = semy_get_pre_release_count(&semver);
    if (count > 0)
    {
        strcat(output, "-");
        for (int32_t i = 0; i < count; i++)
        {
            if (i > 0)
            {
                strcat(output, ".");
            }
            strcat(output, semy_get_pre_release(&semver, i));
        }
    }

    count = semy_get_build_count(&semver);
    if (count > 0)
    {
        strcat(output, "+");
        for (int32_t i = 0; i < count; i++)
        {
            if (i > 0)
            {
                strcat(output, ".");
            }
            strcat(output, semy_get_build(&semver, i));
        }
    }


    ASSERT_STR_EQ(input, output, "incorrect version data extracted: %s", input);
}

TEST(semver, init_exceed_string_length_limit)
{
    semy_error_t err = SEMY_NO_ERROR;
    semy_t semver = {0};
    char input[1002] = {0};

    // Construct an extreamlly long but valid semantic version.
    // In this case "1.0.0-aaaaaaaaa...aaaaaaaaa\0" where the ellipses expands to lots of 'a' characters.
    memset(input, 'a', sizeof(input) - 1);
    memcpy(input, "1.0.0-", 6);

    err = semy_parse(&semver, sizeof(semver), input);
    ASSERT_EQ((int)SEMY_LIMITS_EXCEEDED, err);
}

TEST(semver, init_close_to_but_not_exeeding_string_limit)
{
    semy_error_t err = SEMY_NO_ERROR;
    semy_t semver = {0};
    char input[1001] = {0};

    // Construct an extreamlly long but valid semantic version.
    // In this case "1.0.0-aaaaaaaaa...aaaaaaaaa\0" where the ellipses expands to lots of 'a' characters.
    memset(input, 'a', sizeof(input) - 1);
    memcpy(input, "1.0.0-", 6);

    err = semy_parse(&semver, sizeof(semver), input);
    ASSERT_EQ((int)SEMY_NO_ERROR, err);
}

TEST(semver, init_too_many_pre_release_identifiers)
{
    semy_error_t err = SEMY_NO_ERROR;
    semy_t semver = {0};
    char input[1001] = {0};

    memcpy(input, "1.0.0-", 6);
    for (int i = 6; i < 1000; i++)
    {
        if ((i % 2) == 0)
        {
            input[i] = 'a';
        }
        else
        {
            input[i] = '.';
        }
    }

    err = semy_parse(&semver, sizeof(semver), input);
    ASSERT_EQ((int)SEMY_LIMITS_EXCEEDED, err);
}

TEST(semver, init_too_many_build_identifiers)
{
    semy_error_t err = SEMY_NO_ERROR;
    semy_t semver = {0};
    char input[1001] = {0};

    memcpy(input, "1.0.0+", 6);
    for (int i = 6; i < 1000; i++)
    {
        if ((i % 2) == 0)
        {
            input[i] = 'a';
        }
        else
        {
            input[i] = '.';
        }
    }

    err = semy_parse(&semver, sizeof(semver), input);
    ASSERT_EQ((int)SEMY_LIMITS_EXCEEDED, err);
}

TEST(semver, init_null_string)
{
    semy_t semver = {0};
    const semy_error_t err = semy_parse(NULL, sizeof(semver), "1.0.0");
    ASSERT_EQ((int)SEMY_INVALID_OPERATION, err);
}

TEST(semver, init_null_version)
{
    semy_t semver = {0};
    const semy_error_t err = semy_parse(&semver, sizeof(semver), NULL);
    ASSERT_EQ((int)SEMY_INVALID_OPERATION, err);
}

TEST(semver, init_illegal_size)
{
    semy_t semver = {0};
    const semy_error_t err = semy_parse(&semver, sizeof(semver)-1, "1.0.0");
    ASSERT_EQ((int)SEMY_INVALID_OPERATION, err);
}

TEST(semver, core_version_illegal_arguments)
{
    ASSERT_EQ(-1, semy_get_major(NULL));
    ASSERT_EQ(-1, semy_get_minor(NULL));
    ASSERT_EQ(-1, semy_get_patch(NULL));
}

TEST(semver, pre_release_illegal_arguments)
{
    semy_error_t err = SEMY_NO_ERROR;
    semy_t semver = {0};
    
    err = semy_parse(&semver, sizeof(semver), "1.0.0-alpha");
    ASSERT_EQ((int)SEMY_NO_ERROR, err);

    ASSERT_NULL(semy_get_pre_release(&semver, -1));
    ASSERT_NONNULL(semy_get_pre_release(&semver, 0));
    ASSERT_NULL(semy_get_pre_release(NULL, 0));
    ASSERT_NULL(semy_get_pre_release(&semver, 1));

    ASSERT_EQ(-1, semy_get_pre_release_count(NULL));
}

TEST(semver, build_illegal_arguments)
{
    semy_error_t err = SEMY_NO_ERROR;
    semy_t semver = {0};
    
    err = semy_parse(&semver, sizeof(semver), "1.0.0+build");
    ASSERT_EQ((int)SEMY_NO_ERROR, err);

    ASSERT_NULL(semy_get_build(&semver, -1));
    ASSERT_NONNULL(semy_get_build(&semver, 0));
    ASSERT_NULL(semy_get_build(NULL, 0));
    ASSERT_NULL(semy_get_build(&semver, 1));

    ASSERT_EQ(-1, semy_get_build_count(NULL));
}
