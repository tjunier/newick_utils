cmake_minimum_required(VERSION 2.8)

project(NUTILS C)

enable_testing()

option(USE_LIBXML "Use libXML" ON)
option(USE_LUA	  "Use Lua" ON)

if(USE_LIBXML)
	find_package(LibXml2)
endif(USE_LIBXML)

if(USE_LUA)
	find_package(Lua51)
endif(USE_LUA)

# this must go _before_ any add_subdirectory that needs it...
include_directories(${NUTILS_SOURCE_DIR} /usr/include/libxml2)
add_subdirectory(src)
add_subdirectory(tests)
