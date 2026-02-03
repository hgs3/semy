EXTRA_DIST = autogen.sh LICENSE CMakeLists.txt SemyConfig.cmake.in README.md

AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libsemy.la
noinst_PROGRAMS = semy
include_HEADERS = semy.h

libsemy_la_SOURCES = semy.c semy.h
libsemy_la_LDFLAGS = -no-undefined -version-info 0:0:0

semy_SOURCES = semy-cli.c
semy_CFLAGS = -I$(top_srcdir)
semy_LDADD = libsemy.la
semy_DEPENDENCIES = libsemy.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = semy.pc

man1_MANS = semy.1
