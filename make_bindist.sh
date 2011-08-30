#!/bin/sh

# commands for making binary distributions. We assume that we don't need to run
# autoconf (i.e., configure.ac has not been changed).

# We also assume that LibXML, Lua and Guile are correctly found, when used.
# This is not meant to be run by users, but by me when preparing releases.

# NOTE: it should be possible to do this using automake, but it just doesn't
# seem to work.

make distclean
./configure --disable-shared && \
	make && \
	make check && \
	make EXTRA=enabled bin-dist

make distclean
./configure --disable-shared --without-libxml --without-guile --without-lua && \
	make && \
	make check && \
	make EXTRA=disabled bin-dist
