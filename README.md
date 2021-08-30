The Newick Utilities are a suite of programs for working with Newick-formatted
phylogenetic trees.

Documentation
=============

See in [doc/](doc).

Installing
==========

Binaries
--------

If you have a binaries distribution (that is, with a name like
newick-utils-x.y.z-linux-x86-64.tar.gz), you do not have to compile. The
binaries are in ./src ; they all have names that start with nw_, so you can do 

    $ sudo cp src/nw_* /usr/local/bin	# or wherever you want

You can test the programs by cd'ing to ./tests, and doing the following:

    $ ./test_binaries.sh


Source
------

If you have a source-only distribution, or if you want to build the binaries
yourself, it's the familiar

    $ autoreconf -fi
    $ ./configure
    $ make
    $ make check	# optional but recommended
    $ sudo make install

In addition to a C compiler (I use GCC), you need Flex and Bison (possibly
already on your system); optionally you can use LibXML, GNU Guile, and Lua.

See [the manual](doc/nwutils_tutorial.pdf) for details.
