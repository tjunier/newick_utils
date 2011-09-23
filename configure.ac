#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([Newick Utils],[1.6],[thomas.junier@.unige.ch])
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([src/display.c])

AC_CONFIG_HEADERS([config.h])
AH_TEMPLATE([USE_LIBXML2],[Defined to 1 if all conditions are met for use of LibXML2])

# LibXML

AC_ARG_WITH([libxml],
	[
	 	AS_HELP_STRING([--with-libxml],
		[use libxml2 to handle ornaments @<:@default=yes@:>@])
	],
	[],	# do nothing special if passed 
	[with_libxml=yes]	# if not passed, set to 'yes' (default)
)

# Guile

AC_ARG_WITH([guile],
	[
	 	AS_HELP_STRING([--with-guile],
			[Use Guile (enables nw_sched) @<:@default=yes@:>@])
	],
	[], # default: do nothing
	[with_guile=yes]	# not passed: set to yes (default)
)

# Lua

AC_ARG_WITH([lua],
	[
	 	AS_HELP_STRING([--with-lua],
			[Use Lua (enables nw_luaed) @<:@default=yes@:>@])
	],
	[], # default: do nothing
	[with_lua=yes]	# not passed: set to yes (default)
)

# Checks for programs.
AC_PROG_CC
AC_PROG_YACC
AM_PROG_LEX	# TODO: ensure Flex >= 2.5.35
# TODO: mb check for LaTeX for doc...

LT_INIT#([disable-fast-install)]

# Checks for libraries.
AC_CHECK_LIB([m], [log])

# Checks for header files.

AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS(inttypes.h)
AC_CHECK_HEADERS(libintl.h)
AC_CHECK_HEADERS(malloc.h)
AC_CHECK_HEADERS(stddef.h)
AC_CHECK_HEADERS(stdlib.h)
AC_CHECK_HEADERS(string.h)
AC_CHECK_HEADERS(unistd.h)

AS_IF([test "x$with_libxml" = xyes],
	[
		AC_CHECK_LIB([xml2], [xmlParseMemory], [], [with_libxml=no_lib])
		# Try to get xml2-config to tell us where the headers are.
		# TODO: DON'T touch CPPFLAGS! Use AM_CPPFLAGS instead
		AS_IF([xml2-config --version > /dev/null],
			[CPPFLAGS="$(xml2-config --cflags) $CPPFLAGS "]
		)
		AC_CHECK_HEADERS([libxml/tree.h], [], [with_libxml=no_hdr])
		AC_CHECK_HEADERS([libxml/parser.h], [], [with_libxml=no_hdr])
		AC_CHECK_HEADERS([libxml/xpath.h], [], [with_libxml=no_hdr])
	]
)

AS_IF([test "x$with_guile" = xyes],
	[
		AC_CHECK_HEADERS([libguile.h], [], [with_guile=no_hdr])
		#AS_IF([test "x$with_guile_libdir" != xno], [LDFLAGS="$LDFLAGS -L$with_guile_libdir"])
		AC_CHECK_LIB([guile], [scm_with_guile], [], [with_guile=no_lib])
	]
)

AS_IF([test "x$with_lua" = xyes],
	[
	 	OLD_AM_CPPFLAGS="$AM_CPPFLAGS"
		AM_CPPFLAGS="$AM_CPPFLAGS $CPPFLAGS"
	 	AC_CHECK_HEADERS([lua.h], [], [with_lua=no_hdr])
		AM_CPPFLAGS="$OLD_AM_CPPFLAGS"
		AC_SEARCH_LIBS([luaL_openlibs], [lua5.1 lua-5.1 lua],
					   [], [with_lua=no_lib])
	]
)

AS_CASE([x$with_libxml],
		[xyes],
			[
			 	# This goes into config.h
			 	AC_DEFINE([USE_LIBXML2], 1, [Can use LibXML2])
			],
		[xno],
			[AC_MSG_NOTICE([libXML disabled by user.])],
		[xno_lib],
			[AC_MSG_NOTICE([libXML missing (disabled).])],
		[xno_hdr],
			[AC_MSG_NOTICE([libXML disabled due to missing header.])],
		[AC_MSG_NOTICE([Invalid argument --with-libxml=$with_libxml])]
)

AS_CASE([x$with_guile],
		[xyes],
			[
			 	# This goes into config.h
			 	AC_DEFINE([CHECK_NW_SCHED], 1, [Check nw_sched])
			],
		[xno],
			[AC_MSG_NOTICE([Guile disabled by user.])],
		[xno_hdr],
			[AC_MSG_NOTICE([Guile disabled due to missing header.])],
		[xno_lib],
			[AC_MSG_NOTICE([libguile missing (disabled).])],
		[AC_MSG_NOTICE([Invalid argument --with-guile=$with_guile])]
)
# This is accessible in Makefile.in's
AM_CONDITIONAL([WANT_NW_SCHED], [test x$with_guile = xyes])

AS_CASE([x$with_lua],
		[xyes],
			[
			 	# This goes into config.h
			 	AC_DEFINE([CHECK_NW_LUAED], 1, [Check nw_luaed])
			],
		[xno],
			[AC_MSG_NOTICE([Lua disabled by user.])],
		[xno_hdr],
			[AC_MSG_NOTICE([Lua disabled due to missing header.])],
		[xno_lib],
			[AC_MSG_NOTICE([liblua missing (disabled).])],
		[AC_MSG_NOTICE([Invalid argument --with-lua=$with_lua])]
)
# This is accessible in Makefile.in's
AM_CONDITIONAL([WANT_NW_LUAED], [test x$with_lua = xyes])


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
# NOTE: checking for malloc and realloc causes weird problems on Fedora 13.
# OTOH it should be a very rare system that doesn't have them.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memset rint strdup strpbrk strspn])

AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
AC_OUTPUT
