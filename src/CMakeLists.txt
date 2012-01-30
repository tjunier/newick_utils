add_executable(nw_indent indent.c indent_lex.c)

add_executable(nw_labels labels.c newick_scanner.c newick_parser.c
	tree.c rnode.c list.c rnode_iterator.c hash.c nodemap.c
	masprintf.c link.c parser.c)

add_executable(nw_display display.c text_graph.c canvas.c newick_scanner.c newick_parser.c parser.c tree.c rnode.c list.c hash.c masprintf.c rnode_iterator.c nodemap.c link.c)
