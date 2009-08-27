SUBDIRS = src tests doc
EXTRA_DIST = \
	tests/*_args \
	tests/*.exp \
	tests/*.nw \
	tests/*.sh \
	tests/*.map \
	tests/*.cmap \
	src/*.sh \
	doc/tutorial.pdf \
	doc/Makefile \
	data/*

# set PLATFORM=... when running make dist-bin
dist-bin: distdir
	cp src/nw_* $(distdir)/src
	tardir=$(distdir) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).$(PLATFORM).tar.gz
	$(am__remove_distdir)

#ACLOCAL_AMFLAGS = -I m4
