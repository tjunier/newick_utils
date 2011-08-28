
SUBDIRS = src tests 
EXTRA_DIST = \
	BSD_license \
	tests/*_args \
	tests/*.exp \
	tests/*.nw \
	tests/*.sh \
	tests/*.map \
	tests/*.cmap \
	src/*.sh \
	src/newick_utils.py \
	src/nw_info.py \
	src/*.y src/*.l \
	doc/nwutils_tutorial.pdf \
	data/*

doc/nwutils_tutorial.pdf:
	$(MAKE) -C doc

# Make a binary distribution
# This should be done with
# static libraries - pass option '--disable-shared' to ./configure.

PLATFORM = $(shell uname -s)-$(shell uname -m)

api-doc:
	doxygen

api-doc-latex: api-doc
	$(MAKE) -C latex

show:
	echo $(PLATFORM)

BIN_DIST_FILES = $(COMMON) BSD_license src/nw_* doc/nwutils_tutorial.pdf
bin_dist_dir = $(distdir)-$(PLATFORM)
am__remove_bin_dist_dir = \
  { test ! -d "$(bin_dist_dir)" \
    || { find "$(bin_dist_dir)" -type d ! -perm -200 -exec chmod u+w {} ';' \
         && rm -fr "$(bin_dist_dir)"; }; }

# We want to remake it each time to ensure correct ./configure options
.PHONY: static-bin rm-bin-dist-dir
static-bin:
	$(MAKE) clean
	./configure --disable-shared
	make
	make check

$(bin_dist_dir): static-bin
	rm -rf $(bin_dist_dir)
	mkdir $(bin_dist_dir)
	cp $(BIN_DIST_FILES) $(bin_dist_dir)

bin-dist: $(bin_dist_dir)
	tardir=$(bin_dist_dir) && $(am__tar) \
	       | GZIP=$(GZIP_ENV) gzip -c > $(bin_dist_dir).tar.gz
	$(am__remove_bin_dist_dir)

mydistcheck: dist
	tar xzf $(distdir).tar.gz	
	cd $(distdir) ; ./configure && make check
	rm -rf $(distdir)
	tar xzf $(distdir).tar.gz	
	cd $(distdir) ; ./configure --disable-shared && make check
	rm -rf $(distdir)

