# This compiles all examples at once

cmake_minimum_required(VERSION 2.8)

project( examples )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../libmogi/cmake/Modules/")

add_subdirectory( welcome )
add_subdirectory( head_test )
add_subdirectory( dance )
add_subdirectory( simple_walk )
add_subdirectory( simple_footControl )

add_subdirectory( appInterfaceDemo )
add_subdirectory( dynamixel_examples )

find_package(OpenGL)
if(OPENGL_FOUND)
	add_definitions(-DOPENGL_EXISTS)
	add_subdirectory( openGL )
endif()


