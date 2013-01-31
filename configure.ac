#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([replace_reads], [1.1], [mp15@sanger.ac.uk])
AM_INIT_AUTOMAKE([-Wall foreign])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX([c++ g++])
AC_PROG_CC
AC_PROG_CC_C99

# Checks for libraries.
AC_CHECK_LIB([hts], [hts_open])
if test "$ac_cl_lib_hts" == no
then
	AC_MSG_WARN([No htslib found])
fi
AC_CHECK_LIB([z], [zlib])

# Checks for header files.
CXXFLAGS="-DPOSIX_SOURCE -g -I../../htslib -std=gnu++0x -DPOSIX_SOURCE"
LDFLAGS="-L../../htslib/htslib -lz -lhts"
LIBS="-lz -lhts"

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
