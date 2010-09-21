#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
AC_INIT([Newick Utils],[1.3.5],[thomas.junier@.unige.ch])
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([src])

AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_YACC
AM_PROG_LEX
LT_INIT#([disable-fast-install)]

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])

# Checks for header files.

# try to get info about the location of libxml files using xml2-config
if xml2-config --version > /dev/null; then
	CPPFLAGS="$CPPFLAGS $(xml2-config --cflags)"
else
	CPPFLAGS="$CPPFLAGS -I/usr/include/libxml2"
fi
echo "CPPFLAGS: $CPPFLAGS"

AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADER(libxml/tree.h)
AC_CHECK_HEADER(libxml/parser.h)
AC_CHECK_HEADER(libxml/xpath.h)
AC_CHECK_HEADER(inttypes.h)
AC_CHECK_HEADER(libintl.h)
AC_CHECK_HEADER(malloc.h)
AC_CHECK_HEADER(stddef.h)
AC_CHECK_HEADER(stdlib.h)
AC_CHECK_HEADER(string.h)
AC_CHECK_HEADER(unistd.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memset rint strdup strpbrk strspn])

AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
AC_OUTPUT
