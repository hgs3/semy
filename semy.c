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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_VERSION_LENGTH 200
#define MAX_IDENTIFIERS 25

typedef uint16_t stringIndice_t;

enum
{
    VERSION_CORE_MAJOR,
    VERSION_CORE_MINOR,
    VERSION_CORE_PATCH,
    VERSION_CORE_COUNT,
};

struct preRelease
{
    int32_t numeric_value;
    stringIndice_t string_value;
    int16_t is_alphanumeric;
};

struct semVer
{
    int32_t versions[VERSION_CORE_COUNT];
    uint8_t pre_release_count;
    uint8_t build_metadata_count;
    stringIndice_t chars_allocated;
    struct preRelease pre_release[MAX_IDENTIFIERS];
    stringIndice_t build_metadata[MAX_IDENTIFIERS];
    char strings[244];
};

static_assert(sizeof(struct semVer) == sizeof(semy_t), "expected matching structure size");
static_assert((offsetof(struct semVer, versions[VERSION_CORE_MAJOR]) % 4) == 0, "expected 32-bit alignment");
static_assert((offsetof(struct semVer, versions[VERSION_CORE_MINOR]) % 4) == 0, "expected 32-bit alignment");
static_assert((offsetof(struct semVer, versions[VERSION_CORE_PATCH]) % 4) == 0, "expected 32-bit alignment");
static_assert((offsetof(struct semVer, pre_release_count) % 1) == 0, "expected 8-bit alignment");
static_assert((offsetof(struct semVer, build_metadata_count) % 1) == 0, "expected 8-bit alignment");
static_assert((offsetof(struct semVer, chars_allocated) % 2) == 0, "expected 16-bit alignment");
static_assert((offsetof(struct semVer, pre_release) % 4) == 0, "expected 32-bit alignment");
static_assert((offsetof(struct semVer, build_metadata) % 2) == 0, "expected 16-bit alignment");
static_assert((offsetof(struct semVer, strings) % 1) == 0, "expected 8-bit alignment");

static_assert(sizeof(struct preRelease) == 8, "expected 32-bit size");
static_assert((offsetof(struct preRelease, numeric_value) % 4) == 0, "expected 32-bit alignment");
static_assert((offsetof(struct preRelease, string_value) % 2) == 0, "expected 16-bit alignment");
static_assert((offsetof(struct preRelease, is_alphanumeric) % 2) == 0, "expected 16-bit alignment");

static_assert(sizeof(semy_t) == 512, "expected 0.5 kb");
static_assert(sizeof(semy_error_t) == 4, "expected 4 bytes");

#define CHAR_IS_LETTER 0x1
#define CHAR_IS_DIGIT 0x2
#define CHAR_IS_POSITIVE_DIGIT 0x4
#define CHAR_IS_NON_DIGIT 0x8

static const uint8_t character_flags[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 
    2, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 
    0, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 
    0, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
};

static inline bool is_letter(char c)
{
    return (character_flags[(uint8_t)c] & CHAR_IS_LETTER) == CHAR_IS_LETTER;
}

static inline bool is_digit(char c)
{
    return (character_flags[(uint8_t)c] & CHAR_IS_DIGIT) == CHAR_IS_DIGIT;
}

static inline bool is_positive_digit(char c)
{
    return (character_flags[(uint8_t)c] & CHAR_IS_POSITIVE_DIGIT) == CHAR_IS_POSITIVE_DIGIT;
}

static inline bool is_non_digit(char c)
{
    return (character_flags[(uint8_t)c] & CHAR_IS_NON_DIGIT) == CHAR_IS_NON_DIGIT;
}

static semy_error_t str2int(const char *s, size_t slen, int32_t *value)
{
    uint64_t result = UINT64_C(0);
    for (size_t i = 0; i < slen; i++)
    {
        const char c = s[i];
        assert(c >= '0' && c <= '9');
        result = result * UINT64_C(10) + (uint64_t)(c - '0');
        if (result > INT32_MAX)
        {
            return SEMY_LIMITS_EXCEEDED;
        }
    }

    *value = (int32_t)result;
    return SEMY_NO_ERROR;
}

// <digits> ::= <digit>
//            | <digit> <digits>
static semy_error_t parse_digits(const char *string, size_t *advance)
{
    const char *s = string;

    if (is_digit(*s))
    {
        s += 1;
    }
    else
    {
        return SEMY_BAD_SYNTAX;
    }

    while (is_digit(*s))
    {
        s += 1;
    }

    *advance = s - string;
    return SEMY_NO_ERROR;
}

// <numeric-identifier> ::= "0"
//                        | <positive digit> [ <digits> ]
static semy_error_t parse_numeric_identifier(const char *string, size_t *advance)
{
    const char *s = string;

    if (*s == '0')
    {
        *advance = 1;
        return SEMY_NO_ERROR;
    }

    if (is_positive_digit(*s))
    {
        s += 1;
    }
    else
    {
        return SEMY_BAD_SYNTAX;
    }

    while (is_digit(*s))
    {
        s += 1;
    }

    *advance = s - string;
    return SEMY_NO_ERROR;
}

// <version-core> ::= <major> "." <minor> "." <patch>
static semy_error_t parse_version_core(struct semVer *semver, const char *string, size_t *advance)
{
    const char *s = string;
    size_t len = 0;
    semy_error_t err = SEMY_NO_ERROR;

    // <major> ::= <numeric identifier>
    err = parse_numeric_identifier(s, &len);
    if (err != SEMY_NO_ERROR)
    {
        return err;
    }

    err = str2int(s, len, &semver->versions[VERSION_CORE_MAJOR]);
    if (err != SEMY_NO_ERROR)
    {
        return err;
    }
    s += len;

    // "."
    if (*s != '.')
    {
        return SEMY_BAD_SYNTAX;
    }
    s += 1;

    // <minor> ::= <numeric identifier>
    err = parse_numeric_identifier(s, &len);
    if (err != SEMY_NO_ERROR)
    {
        return err;
    }

    err = str2int(s, len, &semver->versions[VERSION_CORE_MINOR]);
    if (err != SEMY_NO_ERROR)
    {
        return err;
    }
    s += len;

    // "."
    if (*s != '.')
    {
        return SEMY_BAD_SYNTAX;
    }
    s += 1;

    // <patch> ::= <numeric identifier>
    err = parse_numeric_identifier(s, &len);
    if (err != SEMY_NO_ERROR)
    {
        return err;
    }

    err = str2int(s, len, &semver->versions[VERSION_CORE_PATCH]);
    if (err != SEMY_NO_ERROR)
    {
        return err;
    }
    s += len;

    *advance = s - string;
    return SEMY_NO_ERROR;
}

// <identifier-characters> ::= <identifier-character>
//                           | <identifier-character> <identifier-characters>
//
// <identifier-character> ::= <digit>
//                          | <non-digit>
static size_t parse_identifier_characters(const char *string)
{
    const char *s = string;
    while (is_digit(*s) || is_non_digit(*s))
    {
        s += 1;
    }
    return s - string;
}

// <alphanumeric-identifier> ::= <non-digit> [ <identifier-characters> ]
//                             | <identifier-characters> <non-digit> [ <identifier-characters> ]
static semy_error_t parse_alphanumeric_identifier(const char *string, size_t *advance)
{
    const char *s = string;
    size_t len = 0;

    if (is_non_digit(*s))
    {
        s += 1;
        s += parse_identifier_characters(s);
        *advance = s - string;
        return SEMY_NO_ERROR;
    }

    len = parse_identifier_characters(s);
    if (len > 0)
    {
        // Check if there was a non-digit somewhere.
        for (size_t i = 0; i < len; i++)
        {
            if (is_non_digit(s[i]))
            {
                s += len;
                *advance = s - string;
                return SEMY_NO_ERROR;
            }
        }
    }
    return SEMY_BAD_SYNTAX;
}

// <pre-release-identifier> ::= <alphanumeric-identifier>
//                            | <numeric-identifier>
static semy_error_t parse_pre_release_identifier(const char *string, size_t *advance, bool *is_alnum)
{
    const char *s = string;
    size_t len = 0;
    semy_error_t err = SEMY_NO_ERROR;

    err = parse_alphanumeric_identifier(s, &len);
    if (err == SEMY_NO_ERROR)
    {
        s += len;
        *advance = s - string;
        *is_alnum = true;
        return SEMY_NO_ERROR;
    }
    
    if (err == SEMY_BAD_SYNTAX)
    {
        err = parse_numeric_identifier(s, &len);
        if (err == SEMY_NO_ERROR)
        {
            s += len;
            *advance = s - string;
            return SEMY_NO_ERROR;        
        }
    }
    
    return SEMY_BAD_SYNTAX;
}

static uint16_t add_string(struct semVer *semver, const char *s, size_t slen)
{   
    const uint16_t bytes_needed = (uint16_t)(slen + 1); // +1 for the null byte
    const uint16_t bytes_remaining = (uint16_t)(MAX_VERSION_LENGTH - semver->chars_allocated);
    assert(bytes_remaining >= bytes_needed);

    const uint16_t indice = semver->chars_allocated;
    char *sptr = &semver->strings[semver->chars_allocated];
    semver->chars_allocated += (uint16_t)bytes_needed;

    strncpy(sptr, s, slen);
    return indice;
}

static semy_error_t add_pre_release_identifier(struct semVer *semver, bool is_alnum, const char *s, size_t slen)
{
    if (semver->pre_release_count >= MAX_IDENTIFIERS)
    {
        return SEMY_LIMITS_EXCEEDED;
    }

    semy_error_t err = SEMY_NO_ERROR;
    struct preRelease *id = &semver->pre_release[semver->pre_release_count];
    id->is_alphanumeric = is_alnum;
    id->string_value = add_string(semver, s, slen);

    // If this is a numeric identifier, then convert its value to a 32-bit integer.
    if (!is_alnum)
    {
        err = str2int(s, slen, &id->numeric_value);
        if (err != SEMY_NO_ERROR)
        {
            return err;
        }
    }

    semver->pre_release_count += 1;
    return err;
}

// <pre-release> ::= <pre-release-identifier>
//                 | <pre-release-identifier> "." <pre-release>
static semy_error_t parse_pre_release(struct semVer *semver, const char *string, size_t *advance)
{
    const char *s = string;
    size_t len = 0;
    semy_error_t err = SEMY_NO_ERROR;

    for (;;)
    {
        bool is_alnum = false;
        err = parse_pre_release_identifier(s, &len, &is_alnum);
        if (err != SEMY_NO_ERROR)
        {
            return err;
        }

        err = add_pre_release_identifier(semver, is_alnum, s, len);
        if (err != SEMY_NO_ERROR)
        {
            return err;
        }
        s += len;

        if (*s != '.')
        {
            break;
        }
        s += 1;
    }

    *advance = s - string;
    return err;
}

// <build identifier> ::= <alphanumeric-identifier>
//                      | <digits>
static semy_error_t parse_build_metadata(const char *string, size_t *advance)
{
    const char *s = string;
    size_t len = 0;
    semy_error_t err = parse_alphanumeric_identifier(s, &len);
    if (err == SEMY_NO_ERROR)
    {
        s += len;
        *advance = s - string;
    }
    else if (err == SEMY_BAD_SYNTAX)
    {
        err = parse_digits(s, &len);
        if (err == SEMY_NO_ERROR)
        {
            s += len;
            *advance = s - string;
        }
    }
    return err;
}

static semy_error_t add_build_metadata(struct semVer *semver, const char *s, size_t slen)
{
    if (semver->build_metadata_count >= MAX_IDENTIFIERS)
    {
        return SEMY_LIMITS_EXCEEDED;
    }

    semver->build_metadata[semver->build_metadata_count] = add_string(semver, s, slen);    
    semver->build_metadata_count += 1;
    return SEMY_NO_ERROR;
}

// <build> ::= <build-identifier>
//           | <build-identifier> "." <build>
static semy_error_t parse_build(struct semVer *semver, const char *string, size_t *advance)
{
    const char *s = string;
    size_t len = 0;
    semy_error_t err = SEMY_NO_ERROR;

    for (;;)
    {
        err = parse_build_metadata(s, &len);
        if (err != SEMY_NO_ERROR)
        {
            return err;
        }

        err = add_build_metadata(semver, s, len);
        if (err != SEMY_NO_ERROR)
        {
            return err;
        }
        s += len;

        if (*s != '.')
        {
            break;
        }
        s += 1;
    }

    *advance = s - string;
    return err;
}

// <valid semver> ::= <version-core>
//                  | <version-core> [ "-" <pre-release> [ "+" <build> ] ]
static semy_error_t parse_semver(struct semVer *semver, const char *string, size_t *advance)
{
    const char *s = string;
    size_t len = 0;
    semy_error_t err = SEMY_NO_ERROR;

    err = parse_version_core(semver, s, &len);
    if (err != SEMY_NO_ERROR)
    {
        return err;
    }
    s += len;

    if (*s == '-')
    {
        s += 1;
        err = parse_pre_release(semver, s, &len);
        if (err != SEMY_NO_ERROR)
        {
            return err;
        }
        s += len;
    }

    if (*s == '+')
    {
        s += 1;
        err = parse_build(semver, s, &len);
        if (err != SEMY_NO_ERROR)
        {
            return err;
        }
        s += len;
    }

    *advance = s - string;
    return SEMY_NO_ERROR;
}

SEMY_API semy_error_t semy_parse(semy_t *semver, size_t size, const char *version)
{
    if (version == NULL)
    {
        return SEMY_INVALID_OPERATION;
    }

    if (semver == NULL)
    {
        return SEMY_INVALID_OPERATION;
    }

    if (size != sizeof(struct semVer))
    {
        return SEMY_INVALID_OPERATION;
    }

    // Verify the version string is not unnecessarily long.
    // 1 kb is more than enough for any legit version string.
    for (size_t i = 0; version[i] != '\0'; i++)
    {
        if (i >= MAX_VERSION_LENGTH)
        {
            return SEMY_LIMITS_EXCEEDED;
        }
    }

    struct semVer *sv = (struct semVer *)semver->buf;
    static_assert(sizeof(sv->strings) > MAX_VERSION_LENGTH, "expected enough buffer space to contain string content");
    memset(sv, 0, sizeof(sv[0]));

    size_t advance = 0;
    semy_error_t err = parse_semver(sv, version, &advance);
    if (err == SEMY_NO_ERROR)
    {
        // The semantic version string was parsed successfully.
        // Now check for any extraneous characters.
        version += advance;
        if (*version != '\0')
        {
            err = SEMY_BAD_SYNTAX;
        }
    }
    return err;
}

SEMY_API int32_t semy_get_major(const semy_t *semver)
{
    const struct semVer *sv = (const struct semVer *)semver->buf;
    if (sv == NULL)
    {
        return -1;
    }
    return sv->versions[VERSION_CORE_MAJOR];
}

SEMY_API int32_t semy_get_minor(const semy_t *semver)
{
    const struct semVer *sv = (const struct semVer *)semver->buf;
    if (sv == NULL)
    {
        return -1;
    }
    return sv->versions[VERSION_CORE_MINOR];
}

SEMY_API int32_t semy_get_patch(const semy_t *semver)
{
    const struct semVer *sv = (const struct semVer *)semver->buf;
    if (sv == NULL)
    {
        return -1;
    }
    return sv->versions[VERSION_CORE_PATCH];
}

SEMY_API int32_t semy_get_pre_release_count(const semy_t *semver)
{
    const struct semVer *sv = (const struct semVer *)semver->buf;
    if (sv == NULL)
    {
        return -1;
    }
    return sv->pre_release_count;
}

SEMY_API const char *semy_get_pre_release(const semy_t *semver, int32_t index)
{
    const struct semVer *sv = (const struct semVer *)semver->buf;
    if (sv == NULL)
    {
        return NULL;
    }

    if (index < 0 || index >= sv->pre_release_count)
    {
        return NULL;
    }

    const struct preRelease *id = &sv->pre_release[index];
    return &sv->strings[id->string_value];
}

SEMY_API int32_t semy_get_build_count(const semy_t *semver)
{
    const struct semVer *sv = (const struct semVer *)semver->buf;
    if (sv == NULL)
    {
        return -1;
    }
    return sv->build_metadata_count;
}

SEMY_API const char *semy_get_build(const semy_t *semver, int32_t index)
{
    const struct semVer *sv = (const struct semVer *)semver->buf;
    if (semver == NULL)
    {
        return NULL;
    }

    if (index < 0 || index >= sv->build_metadata_count)
    {
        return NULL;
    }

    return &sv->strings[sv->build_metadata[index]];
}

SEMY_API semy_error_t semy_compare(const semy_t *sv1, const semy_t *sv2, int32_t *result)
{
    const struct semVer *a = (const struct semVer *)sv1->buf;
    const struct semVer *b = (const struct semVer *)sv2->buf;

    if (a == NULL)
    {
        return SEMY_INVALID_OPERATION;
    }

    if (b == NULL)
    {
        return SEMY_INVALID_OPERATION;
    }

    if (result == NULL)
    {
        return SEMY_INVALID_OPERATION;
    }

    // Compare the major, minor, and patch versions.
    for (int32_t i = 0; i < VERSION_CORE_COUNT; i++)
    {
        const int32_t v1 = a->versions[i];
        const int32_t v2 = b->versions[i];
        if (v1 < v2)
        {
            *result = -1;
            return SEMY_NO_ERROR;
        }
        else if (v1 > v2)
        {
            *result = 1;
            return SEMY_NO_ERROR;
        }
    }

    // Compute the smallest number of pre-release identifiers to compare.
    int32_t minimum_pre_release_felds = a->pre_release_count;
    if (minimum_pre_release_felds > b->pre_release_count)
    {
        minimum_pre_release_felds = b->pre_release_count;
    }

    // Compare the pre-release versions.
    for (int32_t i = 0; i < minimum_pre_release_felds; i++)
    {
        const struct preRelease *x = &a->pre_release[i];
        const struct preRelease *y = &b->pre_release[i];
        
        // Identifiers consisting of only digits are compared numerically.
        if (x->is_alphanumeric && y->is_alphanumeric)
        {
            const char *s1 = &a->strings[x->string_value];
            const char *s2 = &b->strings[y->string_value];
            char diff;
            do
            {
                diff = *s1 - *s2;
                if (diff != 0)
                {
                    break;
                }
            } while (*s1++ && *s2++);

            if (diff < 0)
            {
                *result = -1;
                return SEMY_NO_ERROR;
            }
            else if (diff > 0)
            {
                *result = 1;
                return SEMY_NO_ERROR;
            }
            continue;
        }

        // Identifiers with letters or hyphens are compared lexically in ASCII sort order.
        if (!x->is_alphanumeric && !y->is_alphanumeric)
        {
            if (x->numeric_value < y->numeric_value)
            {
                *result = -1;
                return SEMY_NO_ERROR;
            }
            else if (x->numeric_value > y->numeric_value)
            {
                *result = 1;
                return SEMY_NO_ERROR;
            }
            continue;
        }

        // Alphanumeric identifiers have greater precedence than numeric identifiers.
        if (!x->is_alphanumeric && y->is_alphanumeric)
        {
            *result = -1;
            return SEMY_NO_ERROR;
        }
        else if (x->is_alphanumeric && !y->is_alphanumeric)
        {
            *result = 1;
            return SEMY_NO_ERROR;
        }
    }

    // A larger set of pre-release fields has a higher precedence than a smaller set,
    // if all of the preceding identifiers are equal.
    if (minimum_pre_release_felds > 0)
    {
        if (a->pre_release_count < b->pre_release_count)
        {
            *result = -1;
            return SEMY_NO_ERROR;
        }
        else if (a->pre_release_count > b->pre_release_count)
        {
            *result = 1;
            return SEMY_NO_ERROR;
        }
    }
    // When major, minor, and patch are equal, a pre-release version has lower
    // precedence than a normal version.
    else if (a->pre_release_count > 0)
    {
        *result = -1;
        return SEMY_NO_ERROR;
    }
    else if (b->pre_release_count > 0)
    {
        *result = 1;
        return SEMY_NO_ERROR;
    }

    // Both versions are semantically equal.
    *result = 0;
    return SEMY_NO_ERROR;
}
