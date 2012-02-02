# Newick parser: Flex and Bison targets

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}	# needed for Flex and Bison below
	)

FLEX_TARGET(IndScanner indent_lex.l
	${CMAKE_CURRENT_BINARY_DIR}/indent_lex.c)
BISON_TARGET(NwParser newick_parser.y
	${CMAKE_CURRENT_BINARY_DIR}/newick_parser.c)
FLEX_TARGET(NwScanner newick_scanner.l
	${CMAKE_CURRENT_BINARY_DIR}/newick_scanner.c)

# nw_clade

add_executable(nw_clade clade.c subtree.c
	newick_scanner.c
	newick_parser.c
	parser.c
	nodemap.c
	rnode_iterator.c
	hash.c
	masprintf.c
	list.c
	rnode.c
	link.c
	lca.c
	error.c
	tree.c
	to_newick.c
	concat.c
	)

# nw_display

if(LIBXML2_FOUND)
	include_directories(${LIBXML2_INCLUDE_DIRS})
endif(LIBXML2_FOUND)

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

target_link_libraries(nw_display m)
if(LIBXML2_FOUND)
	target_link_libraries(nw_display ${LIBXML2_LIBRARIES})
endif(LIBXML2_FOUND)

# nw_indent

add_executable(nw_indent indent.c indent_lex.c)

# nw_labels

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

# nw_luaed

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

# nw_reroot

add_executable(nw_reroot reroot.c
	newick_scanner.c
	newick_parser.c
	parser.c
	list.c
	rnode.c
	link.c
	tree.c
	to_newick.c
	hash.c
	lca.c
	nodemap.c
	masprintf.c
	concat.c
	error.c
	rnode_iterator.c
	)
