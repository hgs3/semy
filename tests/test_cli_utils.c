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

void cli_fprintf(FILE *stream, const char *format, ...)
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

TEST_SETUP(semy)
{
    memset(&captured_stdout, 0, sizeof(captured_stdout));
    memset(&captured_stderr, 0, sizeof(captured_stderr));
}

void run_cli_test(const struct CommandLineTestCase *test_case)
{
    int argc = 0;
    while (test_case->argv[argc])
    {
        argc += 1;
    }

    const int exit_code = cli_main(argc, (char **)test_case->argv);
    EXPECT_EQ(test_case->exit_code, exit_code);

    EXPECT_STR_EQ(test_case->out, captured_stdout.buffer);
    EXPECT_STR_EQ(test_case->err, captured_stderr.buffer);
}
