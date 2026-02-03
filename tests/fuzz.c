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
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    // Ensure the input data is null terminated.
    char *str = calloc(size + 1, sizeof(str[0]));
    memcpy(str, data, size);

    puts(str);

    semy_t semy;
    semy_parse(&semy, sizeof(semy), str);

    free(str);
    return 0;
}
