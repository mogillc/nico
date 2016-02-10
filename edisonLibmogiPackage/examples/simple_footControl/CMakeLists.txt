cmake_minimum_required (VERSION 2.6)

project (simple_footControl)

# The version number
set (simple_footControl_VERSION_MAJOR 0)
set (simple_footControl_VERSION_MINOR 0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules/")

add_executable(simple_footControl src/simple_footControl.cpp)

#	Find Mogi:
find_package (LibMogi REQUIRED)
include_directories(${LIBMOGI_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LIBMOGI_Libraries})

target_link_libraries (simple_footControl ${LIBMOGI_LIBRARIES})
