#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
AC_INIT([Newick Utils],[1.3.5],[thomas.junier@.unige.ch])
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([src])

AC_CONFIG_HEADER([config.h])

# LibXML

AC_ARG_WITH([libxml],
	[
	 	AS_HELP_STRING([--with-libxml],
		[use libxml2 to handle ornaments @<:@default=yes@:>@])
	],
	[],	# do nothing special if passed (but see below in lib section)
	[with_libxml=yes]	# if not passed, set to 'yes' (default)
	)

AC_ARG_WITH([libxml-headers],
	[
	 	AS_HELP_STRING([--with-libxml-headers],
		[specify alternative include directory for libxml headers @<:@default=/usr/include@:>@])
	],
	[
	 	AC_MSG_NOTICE([Note: --with-libxml set to 'yes' as libxml include dirs were provided])
		with_libxml=yes
	]
	)

# Guile

AC_ARG_WITH([guile],
	[
	 	AS_HELP_STRING([--with-guile],
		[compile nw_sched (stream editor for trees, embeds Guile) @<:@default=yes@:>@])
	],
	[],	# do nothing special if passed (but see below in lib section)
	[with_guile=yes]	# if not passed, set to 'yes' (default)
	)

AM_CONDITIONAL([WANT_NW_SCHED], [test x$with_guile != xno])

AC_ARG_WITH([guile-headers],
	[
	 	AS_HELP_STRING([--with-guile-headers],
		[specify alternative include directory for guile headers @<:@default=/usr/include@:>@])
	],
	[
	 	AC_MSG_NOTICE([Note: --with-guile set to 'yes' as guile include dirs were provided])
		with_guile=yes
	]
	)

# Checks for programs.
AC_PROG_CC
AC_PROG_YACC
AM_PROG_LEX
# TODO: mb check for LaTeX for doc...

LT_INIT#([disable-fast-install)]

# Checks for libraries.
AC_CHECK_LIB([m], [log])
AS_IF([test "x$with_libxml" != xno],
	[
	 	# use libxml.
	 	AS_IF([test "x$with_libxml" != xyes],
			[
			 	# look for libxml where user said: in $with_libxml
				LDFLAGS="-L$with_libxml $LDFLAGS"
			],
			[
			 	# try to have xml2-config tell us where libxml is
				if xml2-config --version > /dev/null; then
					LDFLAGS="-L$(xml2-config --prefix) $LDFLAGS"
				fi
				# otherwise just use defaults
			]
		)
		AC_CHECK_LIB([xml2], [xmlParseMemory])
	]
	)

AS_IF([test "x$with_guile" != xno],
	[
	 	# use guile.
	 	AS_IF([test "x$with_guile" != xyes],
			[
			 	# look for guile where user said: in $with_guile
				LDFLAGS="-L$with_guile $LDFLAGS"
			]
		)
		AC_CHECK_LIB([guile])
	]
	)
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
AS_IF([test "$with_libxml_headers" != ""],
	[
	 	# look for headers where user said: in $with_libxml_headers
	 	CPPFLAGS="-I$with_libxml_headers $CPPFLAGS"
	],
	[
		# try to have xml2-config tell us where the libxml headers are
		if xml2-config --version > /dev/null; then
			CPPFLAGS="$(xml2-config --cflags) $CPPFLAGS "
		else
			# a reasonable default
			CPPFLAGS="-I/usr/include/libxml2 $CPPFLAGS"
		fi
	]
	)
AS_IF([test "x$with_libxml" != xno],
	[
		# look for libxml headers IFF with-libxml is not 'no'. Where to look for
		# them is governed by CPPFLAGS (see above).
		AC_CHECK_HEADERS(libxml/tree.h)
		AC_CHECK_HEADERS(libxml/parser.h)
		AC_CHECK_HEADERS(libxml/xpath.h)
	]
)

AS_IF([test "$with_guile_headers" != ""],
	[
	 	# look for headers where user said: in $with_guile_headers
	 	CPPFLAGS="-I$with_guile_headers $CPPFLAGS"
	]
	)

AS_IF([test "x$with_guile" != xno],
	[
		# look for guile headers IFF with-guile is not 'no'. Where to look for
		# them is governed by CPPFLAGS (see above).
		AC_CHECK_HEADERS(libguile.h)
	]
)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memset rint strdup strpbrk strspn])

AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
AC_OUTPUT
