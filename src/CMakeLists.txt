include_directories(${NUTILS_SOURCE_DIR} /usr/include/libxml2)

add_executable(nw_indent indent.c indent_lex.c)

add_executable(nw_labels labels.c
	newick_scanner.c
	newick_parser.c
	tree.c
	rnode.c
	list.c
	rnode_iterator.c
	hash.c nodemap.c
	masprintf.c
	link.c
	parser.c)

add_executable(nw_display display.c text_graph.c canvas.c
	newick_scanner.c
	newick_parser.c
	parser.c
	tree.c
	rnode.c
	list.c
	hash.c
	masprintf.c
	rnode_iterator.c
	nodemap.c
	link.c
	concat.c
	node_pos_alloc.c
	simple_node_pos.c
	graph_common.c
	svg_graph.c
	svg_graph_radial.c
	svg_graph_ortho.c
	lca.c
	error.c
	readline.c
	xml_utils.c
	)

target_link_libraries(nw_display m xml2)


if(LUA51_FOUND)
	include_directories(${LUA51_INCLUDE_DIRS})
	add_executable(nw_luaed lua_tree_editor.c
		newick_scanner.c
		newick_parser.c
		parser.c
		tree.c
		rnode.c
		list.c
		rnode_iterator.c
		link.c
		hash.c
		masprintf.c
		nodemap.c
		to_newick.c
		concat.c
		)
	target_link_libraries(nw_luaed lua)
endif(LUA51_FOUND)

