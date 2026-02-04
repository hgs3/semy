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
#include <stdarg.h>

#define EXIT_BAD_SYNTAX 1
#define EXIT_INVALID_OPTION 2
#define EXIT_GENERAL_ERROR 3

struct semver
{
    const char *string;
    semy_t value;
};

// LCOV_EXCL_START
static void cli_fprintf(FILE *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);
}
// LCOV_EXCL_STOP

static void cli_puts(const char *s)
{
    cli_fprintf(stdout, "%s\n", s);
}

static void *cli_calloc(size_t count, size_t size)
{
    return calloc(count, size);
}

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
    const semy_error_t err = semy_parse(semvar, sizeof(semvar[0]), string);
    if (err == SEMY_BAD_SYNTAX)
    {
        cli_fprintf(stderr, "error: invalid semantic version\n");
        return EXIT_BAD_SYNTAX;
    }
    else if (err == SEMY_LIMITS_EXCEEDED)
    {
        cli_fprintf(stderr, "error: semantic version is too complex for this implementation\n");
        return EXIT_GENERAL_ERROR;
    }
    // LCOV_EXCL_START
    else if (err != SEMY_NO_ERROR)
    {
        cli_fprintf(stderr, "error: internal malfunction\n");
        return EXIT_GENERAL_ERROR;
    }
    // LCOV_EXCL_STOP
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
    struct semver *semvers = cli_calloc(argc, sizeof(semvers[0]));
    if (semvers == NULL)
    {
        cli_fprintf(stderr, "error: memory allocation failed\n");
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
        cli_puts(semvers[i].string);
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
        cli_fprintf(stderr, "error: expected exactly two version strings\n");
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

    cli_fprintf(stdout, "%d\n", result);
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
        cli_fprintf(stderr, "error: invalid format: '%s'\n", format);
        return EXIT_INVALID_OPTION;
    }

    if (argc != 1)
    {
        cli_fprintf(stderr, "error: expected exactly one version string\n");
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
        cli_puts("{");
        cli_fprintf(stdout, "    \"raw\": \"%s\",\n", argv[0]);
        cli_fprintf(stdout, "    \"major\": %d,\n", semy_get_major(&semver));
        cli_fprintf(stdout, "    \"minor\": %d,\n", semy_get_minor(&semver));
        cli_fprintf(stdout, "    \"patch\": %d,\n", semy_get_patch(&semver));

        const int32_t pre_release_count = semy_get_pre_release_count(&semver);
        if (pre_release_count > 0)
        {
            cli_puts("    \"preRelease\": [");
            for (int32_t i = 0; i < pre_release_count; i++)
            {
                cli_fprintf(stdout, "        \"%s\"", semy_get_pre_release(&semver, i));
                if (i < pre_release_count - 1)
                {
                    cli_puts(",");
                }
                else
                {
                    cli_puts("");
                }
            }
            cli_puts("    ],");
        }
        else
        {
            cli_puts("    \"preRelease\": [],");
        }

        const int32_t build_metadata_count = semy_get_build_count(&semver);
        if (build_metadata_count > 0)
        {
            cli_puts("    \"buildMetadata\": [");
            for (int32_t i = 0; i < build_metadata_count; i++)
            {
                cli_fprintf(stdout, "        \"%s\"", semy_get_build(&semver, i));
                if (i < build_metadata_count - 1)
                {
                    cli_puts(",");
                }
                else
                {
                    cli_puts("");
                }
            }
            cli_puts("    ]");
        }
        else
        {
            cli_puts("    \"buildMetadata\": []");
        }

        cli_puts("}");
    }
    else
    {
        cli_puts("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
        cli_puts("<semver>");
        cli_fprintf(stdout, "    <raw>%s</raw>\n", argv[0]);
        cli_fprintf(stdout, "    <major>%d</major>\n", semy_get_major(&semver));
        cli_fprintf(stdout, "    <minor>%d</minor>\n", semy_get_minor(&semver));
        cli_fprintf(stdout, "    <patch>%d</patch>\n", semy_get_patch(&semver));

        const int32_t pre_release_count = semy_get_pre_release_count(&semver);
        if (pre_release_count > 0)
        {
            cli_puts("    <preRelease>");
            for (int32_t i = 0; i < pre_release_count; i++)
            {
                cli_fprintf(stdout, "        <identifier>%s</identifier>\n", semy_get_pre_release(&semver, i));
            }
            cli_puts("    </preRelease>");
        }
        else
        {
            cli_puts("    <preRelease></preRelease>");
        }

        const int32_t build_metadata_count = semy_get_build_count(&semver);
        if (build_metadata_count > 0)
        {
            cli_puts("    <buildMetadata>");
            for (int32_t i = 0; i < build_metadata_count; i++)
            {
                cli_fprintf(stdout, "        <identifier>%s</identifier>\n", semy_get_build(&semver, i));
            }
            cli_puts("    </buildMetadata>");
        }
        else
        {
            cli_puts("    <buildMetadata></buildMetadata>");
        }

        cli_puts("</semver>");
    }

    return EXIT_SUCCESS;
}

static void print_usage(void)
{
    cli_puts("Usage:");
    cli_puts("");
    cli_puts("  semver --compare <version1> <version2>");
    cli_puts("  semver --decompose=<format> <version>");
    cli_puts("  semver --sort <version>...");
    cli_puts("  semver --validate <version>...");
    cli_puts("");
}

static int do_usage(void)
{
    print_usage();
    cli_puts("Run 'semver --help' for more information.");
    cli_puts("");
    return EXIT_SUCCESS;
}

static int do_help(void)
{
    print_usage();
    cli_puts("Options:");
    cli_puts("");
    cli_puts("  -c <v1> <version2>");
    cli_puts("  --compare <version1> <version2>");
    cli_puts("       Compare semantic versions 'v1' and 'v2' and print -1, 0, 1 depending");
    cli_puts("       on if v1 < v2, v1 = v2, v1 > v2 (respectively).");
    cli_puts("");
    cli_puts("  -d<format> <version>");
    cli_puts("  -decompose=<format> <version>");
    cli_puts("       Decompose a semantic version into its identifiers.");
    cli_puts("");
    cli_puts("  -s <versions>...");
    cli_puts("  --sort <versions>...");
    cli_puts("       Sort semantic versions and print them in ascending order on their");
    cli_puts("       own line to stdout.");
    cli_puts("");
    cli_puts("  -V <versions>...");
    cli_puts("  --validate <versions>...");
    cli_puts("       Validate one or more semantic versions. If any semantic version is");
    cli_puts("       invalid, the exit status will be 1.");
    cli_puts("");
    cli_puts("Exit status:");
    cli_puts("  0  if OK,");
    cli_puts("  1  if one or more semantic versions are malformed,");
    cli_puts("  2  if the program arguments are incorrect,");
    cli_puts("  3  if a general error occurred while processing the input.");
    cli_puts("");
    cli_puts("This program is distributed under the MIT License.");
    return EXIT_SUCCESS;
}

static int do_version(void)
{
    cli_puts("1.0.0-alpha");
    return EXIT_SUCCESS;
}

static int cli_main(int argc, char *argv[])
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
            cli_fprintf(stderr, "error: invalid argument '%s'\n", arg);
            return EXIT_INVALID_OPTION;
        }
    }
    return do_usage();
}

#ifndef UNIT_TESTING
int main(int argc, char *argv[])
{
    return cli_main(argc, argv);
}
#endif
