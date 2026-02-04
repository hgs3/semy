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
#include "../semy-cli.c"
#include "test_utils.h"

struct StringBuf
{
    int length;
    char buffer[1024 * 16];
};

static struct StringBuf captured_stdout;
static struct StringBuf captured_stderr;

static int allowed_allocations;

static void my_fprintf(FILE *stream, const char *format, ...)
{
    struct StringBuf *sb = (stream == stdout) ? &captured_stdout : &captured_stderr;
    va_list args, copy;

    va_start(args, format);
    va_copy(copy, args);

    int result = vsnprintf(NULL, 0, format, args);
    va_end(args);
    
    if (result < 0)
    {
        fprintf(stderr, "error: printf() malformed format");
        exit(EXIT_FAILURE);
    }

    if (sb->length + result >= COUNT_OF(sb->buffer))
    {
        fprintf(stderr, "error: printf() buffer lacks space");
        exit(EXIT_FAILURE);
    }

    sb->length += vsprintf(&sb->buffer[sb->length], format, copy);
    va_end(copy);
}

static void *my_calloc(size_t count, size_t size)
{
    if (allowed_allocations > 0)
    {
        allowed_allocations -= 1;
        return calloc(count, size);
    }
    return NULL;
}

TEST_SETUP(semy)
{
    memset(&captured_stdout, 0, sizeof(captured_stdout));
    memset(&captured_stderr, 0, sizeof(captured_stderr));
}

static void std_test(const struct CommandLineTestCase *test_case, const char *test_name, int argc, char **argv)
{
    const int exit_code = cli_main(argc, argv);
    EXPECT_EQ(test_case->exit_code, exit_code, "unexpected exit code: %s", test_name);

    EXPECT_STR_EQ(test_case->out, captured_stdout.buffer, "unexpected stdout for test: %s", test_name);
    EXPECT_STR_EQ(test_case->err, captured_stderr.buffer, "unexpected stderr for test: %s", test_name);
}

static void oom_test(const struct CommandLineTestCase *test_case, const char *test_name, int argc, char **argv)
{
    FAKE(cli_calloc, my_calloc);

    for (int i = 0; i < 100; i++)
    {
        allowed_allocations = i;
        if (cli_main(argc, argv) != EXIT_GENERAL_ERROR)
        {
            return;
        }
    }
    ABORT("expected test to terminate without an OOM exit code: %s", test_name);
}

void run_cli_test(const struct CommandLineTestCase *test_case)
{
    FAKE(cli_fprintf, my_fprintf);

    char cmd[1024] = {0};

    int argc = 0;
    while (test_case->argv[argc])
    {
        if (argc > 0)
        {
            strcat(cmd, " ");
        }
        strcat(cmd, test_case->argv[argc]);
        argc += 1;
    }

    std_test(test_case, cmd, argc, (char **)test_case->argv);
    oom_test(test_case, cmd, argc, (char **)test_case->argv);
}
