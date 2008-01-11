#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([Newick Utils], [0.1], [thomas.junier@medecine.unige.ch])
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([src])

AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_YACC
AM_PROG_LEX

# Unit testing
#PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])
#AM_PATH_CHECK
#AM_PROG_CC_C_O	# got msg from autoreconf saying this is req'd :)

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([rint strdup strpbrk strspn snprintf])

AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
AC_OUTPUT
