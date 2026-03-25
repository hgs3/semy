# Specify the project and version + the minimum automake version required.
AC_PREREQ(2.66)
AC_INIT([semy], [0.9.0])

# Sanity check to verify this is indeed the source directory.
AC_CONFIG_SRCDIR([semy.c])

# Initialize automake and use "foreign" so it doesn't attmpet to look for or
# create a GNU project structure: COPYING, NEWS, etc...
AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC
AM_PROG_AR
AC_PROG_CPP
AC_PROG_INSTALL
LT_INIT([win32-dll])

# Run compilation tests against a C compiler.
AC_LANG([C])

# Check for static_assert (a C11 feature).
AC_MSG_CHECKING([for static_assert])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM([#include <assert.h>], [ 
    static_assert((1 + 1) == 2, "C11 is supported.");
  ])],
  [AC_MSG_RESULT([yes])],
  [AC_MSG_ERROR([static_assert is not supported])]
)

# Check for _Alignas (a C11 feature).
AC_MSG_CHECKING([for _Alignas])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM([#include <stddef.h>], [ 
    _Alignas(max_align_t) char x = 0;
  ])],
  [AC_MSG_RESULT([yes])],
  [AC_MSG_ERROR([_Alignas is not supported])]
)

# Check for _Alignof (a C11 feature).
AC_MSG_CHECKING([for _Alignof])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM([#include <stddef.h>], [ 
    size_t x = _Alignof(max_align_t);
  ])],
  [AC_MSG_RESULT([yes])],
  [AC_MSG_ERROR([_Alignof is not supported])]
)

# Generate output files with macros expanded.
AC_CONFIG_FILES([
  Makefile
  semy.pc
])
AC_OUTPUT
