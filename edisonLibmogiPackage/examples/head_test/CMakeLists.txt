cmake_minimum_required (VERSION 2.6)
set(CMAKE_BUILD_TYPE Debug)
project (head_test)

# The version number
set (head_test_VERSION_MAJOR 0)
set (head_test_VERSION_MINOR 0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../../libmogi/cmake/Modules/")

add_executable(head_test src/head_test.cpp)

#	Find Mogi:
find_package (LibMogi REQUIRED)
include_directories(${LIBMOGI_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LIBMOGI_Libraries})

target_link_libraries (head_test ${LIBMOGI_LIBRARIES})
