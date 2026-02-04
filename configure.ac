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

# Generate output files with macros expanded.
AC_CONFIG_FILES([
  Makefile
  semy.pc
])
AC_OUTPUT
