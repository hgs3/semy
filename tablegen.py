#!/usr/bin/env python3

# SPDX-License-Identifier: MIT
#
# Semy - a Semantic Versioning 2.0.0 processor.
# See https://semver.org/.
#
# Copyright (c) 2025-2026 Henry G. Stratmann III
# Copyright (c) 2025-2026 Semy Contributors
#
# This file is part of Semy, distributed under the MIT License.
# For full terms see the included LICENSE file.

# This script generates a lookup table for classifying C characters
# for the purposes of lexical analysis.

CHAR_IS_LETTER = 0x1
CHAR_IS_DIGIT = 0x2
CHAR_IS_POSITIVE_DIGIT = 0x4
CHAR_IS_NON_DIGIT = 0x8

positive_digits = set(["1", "2", "3", "4", "5", "6", "7", "8", "9"])
letters = set(["A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
               "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
               "U", "V", "W", "X", "Y", "Z", "a", "b", "c", "d",
               "e", "f", "g", "h", "i", "j", "k", "l", "m", "n",
               "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
               "y", "z" ])

print("#define CHAR_IS_LETTER 0x{:X}".format(CHAR_IS_LETTER))
print("#define CHAR_IS_DIGIT 0x{:X}".format(CHAR_IS_DIGIT))
print("#define CHAR_IS_POSITIVE_DIGIT 0x{:X}".format(CHAR_IS_POSITIVE_DIGIT))
print("#define CHAR_IS_NON_DIGIT 0x{:X}".format(CHAR_IS_NON_DIGIT))
print("")

print("static const uint8_t character_flags[] = {", end="")
for i in range(256):
    if (i % 16) == 0:
        print("\n    ", end="")

    value = 0x0
    c = chr(i)
    if c in letters:
        value |= CHAR_IS_LETTER | CHAR_IS_NON_DIGIT
    if c in positive_digits:
        value |= CHAR_IS_DIGIT | CHAR_IS_POSITIVE_DIGIT
    if c == "0":
        value |= CHAR_IS_DIGIT
    if c == "-":
        value |= CHAR_IS_NON_DIGIT

    print(f"{value}, ", end="")
print("\n};")
