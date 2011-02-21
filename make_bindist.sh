#!/bin/sh

# commands for making binary distributions. We assume that we don't need to run
# autoconf (i.e., configure.ac has not been changed).

# We also assume that LibXML and Guile are correctly found, when used. This is
# not meant to be run by users, but by me when preparing releases.

make distclean
./configure --disable-shared && \
	make && \
	#make check && \
	make EXTRA=enabled bin-dist

make distclean
./configure --disable-shared --without-libxml --without-guile && \
	make && \
	#make check && \
	make EXTRA=disabled bin-dist
