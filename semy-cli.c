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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define EXIT_BAD_SYNTAX 1
#define EXIT_INVALID_OPTION 2
#define EXIT_GENERAL_ERROR 3

struct semver
{
    const char *string;
    semy_t value;
};

static int compare_semvers(const void *a, const void *b)
{
    const struct semver *x = (const struct semver *)a;
    const struct semver *y = (const struct semver *)b;
    int32_t result = 0;
    semy_compare(&x->value, &y->value, &result);
    return (int)result;
}

static int parse(const char *string, semy_t *semvar)
{
    // Verify the version string is not unnecessarily long.
    // 1 kb is more than enough for any legit version string.
    for (size_t i = 0; string[i] != '\0'; i++)
    {
        if (i > 1024)
        {
            return EXIT_GENERAL_ERROR;
        }
    }

    const semy_error_t err = semy_parse(semvar, sizeof(semvar[0]), string);
    if (err == SEMY_BAD_SYNTAX)
    {
        fprintf(stderr, "error: invalid semantic version\n");
        return EXIT_BAD_SYNTAX;
    }
    else if (err == SEMY_LIMITS_EXCEEDED)
    {
        fprintf(stderr, "error: semantic version is too complex for this implementation\n");
        return EXIT_GENERAL_ERROR;
    }
    else if (err != SEMY_NO_ERROR)
    {
        fprintf(stderr, "error: internal malfunction\n");
        return EXIT_GENERAL_ERROR;
    }
    return EXIT_SUCCESS;
}

static int do_validate(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        semy_t semver = {0};
        const int r = parse(argv[i], &semver);
        if (r != EXIT_SUCCESS)
        {
            return r;
        }
    }
    return EXIT_SUCCESS;
}

static int do_sort(int argc, char *argv[])
{
    struct semver *semvers = calloc(argc, sizeof(semvers[0]));
    if (semvers == NULL)
    {
        fprintf(stderr, "error: memory allocation failed\n");
        return EXIT_GENERAL_ERROR;
    }

    for (int i = 0; i < argc; i++)
    {
        struct semver *semver = &semvers[i];
        semver->string = argv[i];
        const int r = parse(semver->string, &semver->value);
        if (r != EXIT_SUCCESS)
        {
            return r;
        }
    }

    qsort(semvers, argc, sizeof(semvers[0]), compare_semvers);

    for (int i = 0; i < argc; i++)
    {
        puts(semvers[i].string);
    }

    free(semvers);
    return EXIT_SUCCESS;
}

static int do_compare(int argc, char *argv[])
{
    semy_t semvers[2] = {0};
    semy_error_t err;

    if (argc != 2)
    {
        fprintf(stderr, "error: expected exactly two version strings\n");
        return EXIT_INVALID_OPTION;
    }

    for (int i = 0; i < 2; i++)
    {
        const int r = parse(argv[i], &semvers[i]);
        if (r != EXIT_SUCCESS)
        {
            return r;
        }
    }

    int32_t result = 0;
    semy_compare(&semvers[0], &semvers[1], &result);

    printf("%d\n", result);
    return EXIT_SUCCESS;
}

static int do_decompose(const char *format, int argc, char *argv[])
{
    enum { INVALID, JSON, XML } target = INVALID;

    if (strcmp(format, "json") == 0)
    {
        target = JSON;
    }
    else if (strcmp(format, "xml") == 0)
    {
        target = XML;
    }
    else
    {
        fprintf(stderr, "error: invalid format: '%s'\n", format);
        return EXIT_INVALID_OPTION;
    }

    if (argc != 1)
    {
        fprintf(stderr, "error: expected exactly one version string\n");
        return EXIT_INVALID_OPTION;
    }

    semy_t semver = {0};
    const int r = parse(argv[0], &semver);
    if (r != EXIT_SUCCESS)
    {
        return r;
    }

    if (target == JSON)
    {
        puts("{");
        printf("    \"raw\": \"%s\",\n", argv[0]);
        printf("    \"major\": %d,\n", semy_get_major(&semver));
        printf("    \"minor\": %d,\n", semy_get_minor(&semver));
        printf("    \"patch\": %d,\n", semy_get_patch(&semver));

        const int32_t pre_release_count = semy_get_pre_release_count(&semver);
        if (pre_release_count > 0)
        {
            puts("    \"preRelease\": [");
            for (int32_t i = 0; i < pre_release_count; i++)
            {
                printf("        \"%s\"", semy_get_pre_release(&semver, i));
                if (i < pre_release_count - 1)
                {
                    puts(",");
                }
                else
                {
                    puts("");
                }
            }
            puts("    ],");
        }
        else
        {
            puts("    \"preRelease\": [],");
        }

        const int32_t build_metadata_count = semy_get_build_count(&semver);
        if (build_metadata_count > 0)
        {
            puts("    \"buildMetadata\": [");
            for (int32_t i = 0; i < build_metadata_count; i++)
            {
                printf("        \"%s\"", semy_get_build(&semver, i));
                if (i < build_metadata_count - 1)
                {
                    puts(",");
                }
                else
                {
                    puts("");
                }
            }
            puts("    ]");
        }
        else
        {
            puts("    \"buildMetadata\": []");
        }

        puts("}");
    }
    else
    {
        puts("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
        puts("<semver>");
        printf("    <raw>%s</raw>\n", argv[0]);
        printf("    <major>%d</major>\n", semy_get_major(&semver));
        printf("    <minor>%d</minor>\n", semy_get_minor(&semver));
        printf("    <patch>%d</patch>\n", semy_get_patch(&semver));

        const int32_t pre_release_count = semy_get_pre_release_count(&semver);
        if (pre_release_count > 0)
        {
            puts("    <preRelease>");
            for (int32_t i = 0; i < pre_release_count; i++)
            {
                printf("        <identifier>%s</identifier>\n", semy_get_pre_release(&semver, i));
            }
            puts("    </preRelease>");
        }
        else
        {
            puts("    <preRelease></preRelease>");
        }

        const int32_t build_metadata_count = semy_get_build_count(&semver);
        if (build_metadata_count > 0)
        {
            puts("    <buildMetadata>");
            for (int32_t i = 0; i < build_metadata_count; i++)
            {
                printf("        <identifier>%s</identifier>\n", semy_get_build(&semver, i));
            }
            puts("    </buildMetadata>");
        }
        else
        {
            puts("    <buildMetadata></buildMetadata>");
        }

        puts("</semver>");
    }

    return EXIT_SUCCESS;
}

static void print_usage(void)
{
    puts("Usage:");
    puts("");
    puts("  semver --compare <version1> <version2>");
    puts("  semver --decompose=<format> <version>");
    puts("  semver --sort <version>...");
    puts("  semver --validate <version>...");
    puts("");
}

static int do_usage(void)
{
    print_usage();
    puts("Run 'semver --help' for more information.");
    puts("");
    return EXIT_SUCCESS;
}

static int do_help(void)
{
    print_usage();
    puts("Options:");
    puts("");
    puts("  -c <v1> <version2>");
    puts("  --compare <version1> <version2>");
    puts("       Compare semantic versions 'v1' and 'v2' and print -1, 0, 1 depending");
    puts("       on if v1 < v2, v1 = v2, v1 > v2 (respectively).");
    puts("");
    puts("  -d<format> <version>");
    puts("  -decompose=<format> <version>");
    puts("       Decompose a semantic version into its identifiers.");
    puts("");
    puts("  -s <versions>...");
    puts("  --sort <versions>...");
    puts("       Sort semantic versions and print them in ascending order on their");
    puts("       own line to stdout.");
    puts("");
    puts("  -V <versions>...");
    puts("  --validate <versions>...");
    puts("       Validate one or more semantic versions. If any semantic version is");
    puts("       invalid, the exit status will be 1.");
    puts("");
    puts("Exit status:");
    puts("  0  if OK,");
    puts("  1  if one or more semantic versions are malformed,");
    puts("  2  if the program arguments are incorrect,");
    puts("  3  if a general error occurred while processing the input.");
    puts("");
    puts("This program is distributed under the MIT License.");
    return EXIT_SUCCESS;
}

static int do_version(void)
{
    puts("1.0.0-alpha");
    return EXIT_SUCCESS;
}

int cli_main(int argc, char *argv[])
{
    if (argc > 1)
    {
        const char *arg = argv[1];

        if (strcmp(arg, "-h") == 0 ||
            strcmp(arg, "--help") == 0)
        {
            return do_help();
        }

        if (strcmp(arg, "-v") == 0 ||
            strcmp(arg, "--version") == 0)
        {
            return do_version();
        }

        if (strcmp(arg, "-c") == 0 ||
            strcmp(arg, "--compare") == 0)
        {
            return do_compare(argc - 2, argv + 2);
        }

        if (strcmp(arg, "-s") == 0 ||
            strcmp(arg, "--sort") == 0)
        {
            return do_sort(argc - 2, argv + 2);
        }
            
        if (strncmp(arg, "-d", 2) == 0 ||
            strncmp(arg, "--decompose=", 12) == 0)
        {
            if (arg[1] == 'd')
            {
                arg += 2;
            }
            else
            {
                arg += 12;
            }
            return do_decompose(arg, argc - 2, argv + 2);
        }

        if (strcmp(arg, "-V") == 0 ||
            strcmp(arg, "--validate") == 0)
        {
            return do_validate(argc - 2, argv + 2);
        }
        
        if (arg[0] == '-')
        {
            fprintf(stderr, "error: invalid argument '%s'\n", arg);
            return EXIT_INVALID_OPTION;
        }
    }
    return do_usage();
}

int main(int argc, char *argv[])
{
    return cli_main(argc, argv);
}
