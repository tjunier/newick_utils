include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Unit (=function) tests

set(UNIT_TESTS
	concat
	error
	hash
	lca
	link
	list
	masprintf
	newick_parser
	newick_scanner
	nodemap
	rnode
	rnode_iterator
	to_newick
	tree
	)

foreach(unit_test ${UNIT_TESTS})
	add_executable(test_${unit_test} test_${unit_test}.c tree_stubs.c)
	target_link_libraries(test_${unit_test} nutils)
	add_test(${unit_test} test_${unit_test})
endforeach(unit_test)

# special cases (i.e., link to some files not in nutils.so and/or need additional libraries)
# TODO  check if tree_stubs.c is always needed

add_executable(test_canvas test_canvas.c ${SRC_DIR}/canvas.c tree_stubs.c)
target_link_libraries(test_canvas nutils)
add_test(canvas test_canvas)

add_executable(test_enode test_enode.c ${SRC_DIR}/enode.c tree_stubs.c)
target_link_libraries(test_enode nutils)
add_test(enode test_enode)

add_executable(test_graph_common test_graph_common.c ${SRC_DIR}/graph_common.c tree_stubs.c)
target_link_libraries(test_graph_common nutils m)
add_test(graph_common test_graph_common)

add_executable(test_node_set test_node_set.c ${SRC_DIR}/node_set.c tree_stubs.c)
target_link_libraries(test_node_set nutils m)
add_test(node_set test_node_set)

add_executable(test_order_tree test_order_tree.c ${SRC_DIR}/order_tree.c tree_stubs.c)
target_link_libraries(test_order_tree nutils m)
add_test(order_tree test_order_tree)

add_executable(test_readline test_readline.c ${SRC_DIR}/readline.c tree_stubs.c)
target_link_libraries(test_readline nutils m)
add_test(readline test_readline)

add_executable(test_subtree test_subtree.c ${SRC_DIR}/subtree.c tree_stubs.c)
target_link_libraries(test_subtree nutils m)
add_test(subtree test_subtree)

add_executable(test_svg_graph_radial test_svg_graph_radial.c
	${SRC_DIR}/svg_graph_radial.c
	${SRC_DIR}/svg_graph_ortho.c
	${SRC_DIR}/svg_graph.c 
	${SRC_DIR}/graph_common.c 
	${SRC_DIR}/readline.c
	${SRC_DIR}/xml_utils.c
	)
target_link_libraries(test_svg_graph_radial nutils m)
add_test(svg_graph_radial test_svg_graph_radial)

add_executable(test_tree_models test_tree_models.c ${SRC_DIR}/tree_models.c tree_stubs.c)
target_link_libraries(test_tree_models nutils m)
add_test(tree_models test_tree_models)

add_executable(test_xml_utils test_xml_utils.c ${SRC_DIR}/xml_utils.c tree_stubs.c)
target_link_libraries(test_xml_utils nutils m)
add_test(xml_utils test_xml_utils)

# Application tests

set(TESTS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(APP_TESTS
	nw_clade
	nw_labels
	nw_luaed
	nw_reroot
	)

foreach(app_test ${APP_TESTS})
	add_test(${app_test}
			${TESTS_SRC_DIR}/test_${app_test}.sh
			${TESTS_SRC_DIR}
			${CMAKE_CURRENT_BINARY_DIR}
			${CMAKE_BINARY_DIR}/src
			)
endforeach(app_test)
