cmake_minimum_required(VERSION 2.8)

project(NUTILS C)

# this must go _before_ any add_subdirectory that needs it...
include_directories(${NUTILS_SOURCE_DIR} /usr/include/libxml2)
add_subdirectory(src)
