# Semy

Semy (rhythms with "remy") is a [semantic version](https://semver.org/) parser and processor written in C11 with no external dependencies and no dynamic memory allocation.

[![Build Status](https://github.com/hgs3/semy/actions/workflows/build.yml/badge.svg)](https://github.com/hgs3/semy/actions/workflows/build.yml)

## Features

* Parse semantic versions
* Extract version identifiers
* Check if two versions are equal
* Check if a version is newer or older than another

## How Semy is Tested

* Unit tests with 100% code coverage
* Fuzz tests (libFuzzer and AFL++)
* Code sanitizers (UBSAN, ASAN, and MSAN)
* Extensive run-time checks

## Comand-Line Interface

Semy includes a command-line interface for validating, comparing, sorting and decomposing semantic versions.

### Validate Semantic Versions

Check if one or more semantic versions are well-formed according to the Semantic Version 2.0.0 specification.

```bash
$ semy -v 1.0.0-beta
$ echo $?
0
$ semy -v 1.0.0beta
$ echo $?
1
```

### Sort Semantic Versions

Two or more semantic versions can be sorted with the -s option.
The sorted semantic versions are printed in ascending order on their own line.

```bash
$ semy -s 2.0.0 2.0.0-rc.1 2.0.0-beta.11 2.0.0-beta.2 \
          2.0.0-beta 2.0.0-alpha.beta 2.0.0-alpha.1 2.0.0-alpha
2.0.0-alpha
2.0.0-alpha.1
2.0.0-alpha.beta
2.0.0-beta
2.0.0-beta.2
2.0.0-beta.11
2.0.0-rc.1
2.0.0
```

### Compare Semantic Versions

Semantic versions 'v1' and 'v2' can be compared with the -c option.
The integer -1, 0, 1 is printed depending on if v1 < v2, v1 = v2, v1 > v2.

```bash
$ semy -c 0.9.1 0.10.0
-1
$ semy -c 2.0.0 2.0.0
0
$ semy -c 1.0.0-beta 1.0.0-alpha
1
```

### Decompose a Semantic Version

A semantic version can be decomposed into its version identifiers with the -c option.
You can decompose a version to either JSON or XML.

```bash
$ semy -djson 1.2.3-beta
{
    "raw": "1.2.3-beta",
    "major": 1,
    "minor": 2,
    "patch": 3,
    "preRelease": [
        "beta"
    ],
    "buildMetadata": []
}
```

## C API

The following C code example prints the major, minor, and patch version of a semantic version.
See [semy.h](semy.h) for documentation.

```c
#include <semy.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    semy_t semver;
    semy_parse(&semver, sizeof(semver), "1.0.0");

    printf("major: %d\n", semy_get_major(&semver));
    printf("minor: %d\n", semy_get_minor(&semver));
    printf("patch: %d\n", semy_get_patch(&semver));

    return 0;
}
```

## Building

To build Semy, download the latest version from the [releases page](https://github.com/hgs3/semy/releases) and build with

```
$ ./configure
$ make
$ make install
```

or build with [CMake](https://cmake.org/)

```
$ cmake -B build
$ cmake --build build
$ cmake --install build
```

Alternatively, you can vendor `semy.c` and `semy.h` in your project.

## License

MIT License.
See [LICENSE](LICENSE) for details.
