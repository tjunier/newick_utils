cmake_minimum_required(VERSION 2.8)

project(NUTILS C)

if (NOT DEFINED BUILD_SHARED_LIBS)
	set(BUILD_SHARED_LIBS ON)
endif(NOT DEFINED BUILD_SHARED_LIBS)

set(CMAKE_C_FLAGS_DEBUG "-O0 -g")

# this doesn't seem to work...
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(BUILD_SHARED_LIBS OFF)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

enable_testing()

# Packaging
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Shell tools for processing phylogenies")
set(CPACK_PACKAGE_FILE_NAME "Newick_Utils-1.6")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Thomas Junier")
set(CPACK_DEBIAN_VERSION_MAJOR 1)
set(CPACK_DEBIAN_VERSION_MINOR 6)
set(CPACK_GENERATOR "TGZ;TBZ2;DEB;RPM")

include(CPack)

option(USE_LIBXML "Use libXML" ON)
option(USE_LUA	  "Use Lua" ON)

find_package(BISON)
find_package(FLEX)

if(USE_LIBXML)
	find_package(LibXml2)
endif(USE_LIBXML)

if(USE_LUA)
	find_package(Lua51)
endif(USE_LUA)

configure_file (
	"${PROJECT_SOURCE_DIR}/CMakeConfig.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
)

# this must go _before_ any add_subdirectory that needs it...
# TODO actually this may no longer be needed
include_directories(${PROJECT_BINARY_DIR} /usr/include/libxml2)
add_subdirectory(src)
add_subdirectory(tests)
