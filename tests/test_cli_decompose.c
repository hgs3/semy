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
        {"semy", "--decompose=json", "1.2.3"},
        0,
        "{\n"
        "    \"raw\": \"1.2.3\",\n"
        "    \"major\": 1,\n"
        "    \"minor\": 2,\n"
        "    \"patch\": 3,\n"
        "    \"preRelease\": [],\n"
        "    \"buildMetadata\": []\n"
        "}\n",
        "",
    },
    {
        {"semy", "-djson", "1.2.3"},
        0,
        "{\n"
        "    \"raw\": \"1.2.3\",\n"
        "    \"major\": 1,\n"
        "    \"minor\": 2,\n"
        "    \"patch\": 3,\n"
        "    \"preRelease\": [],\n"
        "    \"buildMetadata\": []\n"
        "}\n",
        "",
    },
    {
        {"semy", "-djson", "1.2.3-alpha.1+build.20120313144700"},
        0,
        "{\n"
        "    \"raw\": \"1.2.3-alpha.1+build.20120313144700\",\n"
        "    \"major\": 1,\n"
        "    \"minor\": 2,\n"
        "    \"patch\": 3,\n"
        "    \"preRelease\": [\n"
        "        \"alpha\",\n"
        "        \"1\"\n"
        "    ],\n"
        "    \"buildMetadata\": [\n"
        "        \"build\",\n"
        "        \"20120313144700\"\n"
        "    ]\n"
        "}\n",
        "",
    },
    {
        {"semy", "-dxml", "1.2.3"},
        0,
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<semver>\n"
        "    <raw>1.2.3</raw>\n"
        "    <major>1</major>\n"
        "    <minor>2</minor>\n"
        "    <patch>3</patch>\n"
        "    <preRelease></preRelease>\n"
        "    <buildMetadata></buildMetadata>\n"
        "</semver>\n",
        "",
    },
    {
        {"semy", "-dxml", "1.2.3-alpha.1+build.20120313144700"},
        0,
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<semver>\n"
        "    <raw>1.2.3-alpha.1+build.20120313144700</raw>\n"
        "    <major>1</major>\n"
        "    <minor>2</minor>\n"
        "    <patch>3</patch>\n"
        "    <preRelease>\n"
        "        <identifier>alpha</identifier>\n"
        "        <identifier>1</identifier>\n"
        "    </preRelease>\n"
        "    <buildMetadata>\n"
        "        <identifier>build</identifier>\n"
        "        <identifier>20120313144700</identifier>\n"
        "    </buildMetadata>\n"
        "</semver>\n",
        "",
    },
    {
        {"semy", "-dxml", "1.2"},
        1,
        "",
        "error: invalid semantic version\n",
    },
    {
        {"semy", "-dxml"},
        2,
        "",
        "error: expected exactly one version string\n",
    },
    {
        {"semy", "-dyaml"},
        2,
        "",
        "error: invalid format: 'yaml'\n",
    },
};

TEST(semy, cli, .iterations=COUNT_OF(test_cases))
{
    run_cli_test(&test_cases[TEST_ITERATION]);
}
