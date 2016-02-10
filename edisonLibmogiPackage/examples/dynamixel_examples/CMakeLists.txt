cmake_minimum_required (VERSION 2.6)

project ( dynamixel_examples )

# The version number
set (dynamixel_examples_VERSION_MAJOR 0)
set (dynamixel_examples_VERSION_MINOR 0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules/")

add_executable(dynamixel_examples src/dynamixel_examples.cpp)

#       Find Mogi:
find_package (LibMogi REQUIRED)
include_directories(${LIBMOGI_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LIBMOGI_Libraries})

target_link_libraries ( dynamixel_examples ${LIBMOGI_LIBRARIES})

