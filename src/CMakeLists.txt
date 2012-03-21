# Newick parser: Flex and Bison targets

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}	# needed for Flex and Bison below
	${CMAKE_CURRENT_BINARY_DIR}	# needed for Flex and Bison below
	)

FLEX_TARGET(IndScanner indent_lex.l
	${CMAKE_CURRENT_BINARY_DIR}/indent_lex.c)

BISON_TARGET(NwParser newick_parser.y
	${CMAKE_CURRENT_BINARY_DIR}/newick_parser.c)
FLEX_TARGET(NwScanner newick_scanner.l
	${CMAKE_CURRENT_BINARY_DIR}/newick_scanner.c)

# Seems to work w/o this one
#ADD_FLEX_BISON_DEPENDENCY(NwScanner NwParser)

FLEX_TARGET(AddrScanner address_scanner.l
	${CMAKE_CURRENT_BINARY_DIR}/address_scanner.c)
BISON_TARGET(AddrParser address_parser.y
	${CMAKE_CURRENT_BINARY_DIR}/address_parser.c)

# newick utilities library

add_library(nutils SHARED
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

# simple cases 

set(NUTILS_APPS
	condense
	duration
	labels
	prune
	reroot
	stats
	topology
	trim
	)

foreach(app ${NUTILS_APPS})
	add_executable(nw_${app} ${app}.c)
	target_link_libraries(nw_${app} nutils)
endforeach(app)

# Unfortunately, the simple cases are a minority... the others either have
# additional object files, or libs, or are optional in some way, or have
# main() in a file that is not derivable from the executable's name (this
# should be changed, BTW).

# nw_clade: has an additional object file

add_executable(nw_clade clade.c subtree.c)
target_link_libraries(nw_clade nutils)

# nw_display: needs other object files and has optional libs

if(LIBXML2_FOUND)
	include_directories(${LIBXML2_INCLUDE_DIRS})
endif(LIBXML2_FOUND)

add_executable(nw_display display.c text_graph.c canvas.c
	node_pos_alloc.c
	simple_node_pos.c
	graph_common.c
	svg_graph.c
	svg_graph_radial.c
	svg_graph_ortho.c
	readline.c
	xml_utils.c
	)

target_link_libraries(nw_display m nutils)

if(LIBXML2_FOUND)
	target_link_libraries(nw_display ${LIBXML2_LIBRARIES})
endif(LIBXML2_FOUND)

# nw_distance: other object files

add_executable(nw_distance distance.c node_pos_alloc.c simple_node_pos.c)
target_link_libraries(nw_distance nutils)

# nw_ed: other object files

add_executable(nw_ed tree_editor.c enode.c address_scanner.c address_parser.c)
target_link_libraries(nw_ed nutils)

# nw_gen: other obj file (and non-derivable name)

add_executable(nw_gen generate.c tree_models.c)
target_link_libraries(nw_gen m nutils)

# nw_indent: has a additional object file (own scanner)

add_executable(nw_indent indent.c indent_lex.c)

# nw_luaed: is optional and has external libs

if(LUA51_FOUND)
	include_directories(${LUA51_INCLUDE_DIRS})
	add_executable(nw_luaed lua_tree_editor.c)
	target_link_libraries(nw_luaed lua nutils)
endif(LUA51_FOUND)

# nw_match: other obj file 

add_executable(nw_match match.c order_tree.c)
target_link_libraries(nw_match nutils)

# nw_order: other obj file 

add_executable(nw_order order.c order_tree.c)
target_link_libraries(nw_order nutils)

# nw_rename: other obj file

add_executable(nw_rename rename.c readline.c)
target_link_libraries(nw_rename nutils)

# nw_support: other obj file

add_executable(nw_support support.c node_set.c)
target_link_libraries(nw_support m nutils)

# TODO: add nw_sched, nw_luaed, etc iff Scheme, Lua, etc used.
set(PROGRAMS nw_clade nw_condense nw_display nw_distance nw_duration nw_ed
	nw_gen nw_indent nw_labels nw_luaed nw_match nw_order
	nw_prune nw_rename nw_reroot nw_stats nw_support nw_topology
	nw_trim)

install(TARGETS ${PROGRAMS} nutils
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	)
