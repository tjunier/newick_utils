include_directories(${CMAKE_SOURCE_DIR}/src)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(TESTS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(test_rnode test_rnode.c tree_stubs.c
	${SRC_DIR}/rnode.c
	${SRC_DIR}/hash.c
	${SRC_DIR}/masprintf.c
	${SRC_DIR}/list.c
	${SRC_DIR}/rnode_iterator.c
	${SRC_DIR}/nodemap.c
	${SRC_DIR}/link.c
	)


add_test(rnode test_rnode)
add_test(nw_labels ${TESTS_SRC_DIR}/test_nw_labels.sh
		${TESTS_SRC_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		)
