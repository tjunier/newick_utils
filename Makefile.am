SUBDIRS = src tests doc
EXTRA_DIST = \
	tests/*_args \
	tests/*.exp \
	tests/*.nw \
	tests/*.sh \
	tests/*.map \
	tests/*.cmap \
	src/*.sh \
	src/newick_utils.py \
	src/nw_info.py \
	doc/tutorial.pdf \
	doc/Makefile \
	data/*

# Make a binary distribution
# set PLATFORM=<your current platform>. Also, this should be done with
# static libraries - pass option '--disable-shared' to ./configure.

bin-dist: distdir
	cp src/nw_* $(distdir)/src
	tardir=$(distdir) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).$(PLATFORM).tar.gz
	$(am__remove_distdir)

#ACLOCAL_AMFLAGS = -I m4
