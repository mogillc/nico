cmake_minimum_required (VERSION 2.6)

project (welcome)

# The version number
set (welcome_VERSION_MAJOR 0)
set (welcome_VERSION_MINOR 0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../../libmogi/cmake/Modules/")

add_executable(welcome src/welcome.cpp)

#	Find Mogi:
find_package (LibMogi REQUIRED)
include_directories(${LIBMOGI_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LIBMOGI_Libraries})

target_link_libraries (welcome ${LIBMOGI_LIBRARIES})
