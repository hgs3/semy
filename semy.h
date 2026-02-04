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

#ifndef SEMY_H
#define SEMY_H

#include <stdint.h>
#include <stddef.h>

#if defined(_WIN32) || defined(__CYGWIN__)
    #if defined(DLL_EXPORT)
        #define SEMY_API __declspec(dllexport)
    #elif defined(BABEL_STATIC)
        #define SEMY_API
    #else
        #define SEMY_API __declspec(dllimport)
    #endif
#else
    #define SEMY_API
#endif

typedef enum semy_error
{
    SEMY_NO_ERROR,                  // The operation was successful.
    SEMY_INVALID_OPERATION,         // The API was misused (e.g. null was passed where non-null was expected).
    SEMY_BAD_SYNTAX,                // The semantic version string is invalid.
    SEMY_LIMITS_EXCEEDED,           // An implementation limit was hit (e.g. integer overflow, too many build identifiers).
    SEMY_FORCE_32_BIT = 0x7FFFFFFF, // PRIVATE: Do not touch!
} semy_error_t;

typedef struct semy
{
    char _reserved[2048];
} semy_t;

// This function parses the 'version' string and populates the 'semy' structure with the results.
// The 'size' parameter must be equal to the sizeof the 'semy_t' structure.
SEMY_API semy_error_t semy_parse(semy_t *semy, size_t size, const char *version);

// The function populates the 'result' with either -1, 0, or 1 depending on if v1 < v2, v1 = v2, v1 > v2.
SEMY_API semy_error_t semy_compare(const semy_t *v1, const semy_t *v2, int32_t *result);

// These functions return -1 if 'semy' is null.
SEMY_API int32_t semy_get_major(const semy_t *semy);
SEMY_API int32_t semy_get_minor(const semy_t *semy);
SEMY_API int32_t semy_get_patch(const semy_t *semy);
SEMY_API int32_t semy_get_pre_release_count(const semy_t *semy);
SEMY_API int32_t semy_get_build_count(const semy_t *semy);

// These functions return null if 'semy' is null or 'index' is out-of-bounds.
// The pointers returned by these functions belong to the 'semy' structure and
// are considered invalid if the structure is modified in any capacity.
SEMY_API const char *semy_get_pre_release(const semy_t *semy, int32_t index);
SEMY_API const char *semy_get_build(const semy_t *semy, int32_t index);

#endif
