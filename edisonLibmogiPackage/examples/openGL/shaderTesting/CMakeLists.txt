cmake_minimum_required (VERSION 2.8)

project (bump)
set(CMAKE_BUILD_TYPE Debug)

# The version number
set (bump_VERSION_MAJOR 0)
set (bump_VERSION_MINOR 0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/../STL
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/../Shaders
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/../Pics
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

FILE( GLOB SRCS *.h *.cpp )



add_library(camera SHARED camera.cpp)
add_library(vector3d SHARED vector3d.cpp)

add_executable(bump bump.cpp)

target_link_libraries(bump camera vector3d)
target_link_libraries(camera vector3d)




find_package (LibPng REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
set(LIBS ${LIBS} ${PNG_LIBRARY})

find_package (LibSdl REQUIRED)
include_directories(${SDL_INCLUDE_DIR})
set(LIBS ${LIBS} ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY})


#	Find Hexapod:
find_package (LibHexapod REQUIRED)
include_directories(${LIBHEXAPOD_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LIBHEXAPOD_Libraries})

#	Find Dynamixel:
find_package (LibDynamixel REQUIRED)
include_directories(${LIBDYNAMIXEL_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LIBDYNAMIXEL_Libraries})

#	Find Sixaxis:
find_package (LibSixaxis REQUIRED)
include_directories(${LIBSIXAXIS_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LIBSIXAXIS_Libraries})

#	Find Bmath:
find_package (LibBmath REQUIRED)
include_directories(${LIBBMATH_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LIBBMATH_Libraries})

#	include FTDI:
find_package (LibFtdi REQUIRED)
include_directories(${LIBFTDI_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LIBFTDI_Libraries})

#	Include Flycapture:
#find_package (LibFlycapture REQUIRED)
#include_directories(${LIBFLYCAPTURE_INCLUDE_DIRS})
#set(LIBS ${LIBS} ${LIBFLYCAPTURE_Libraries})

#	Include OpenCV:
#find_package (OpenCV REQUIRED)
#include_directories(${OpenCV_INCLUDE_DIRS})
#set(LIBS ${LIBS} ${OpenCV_LIBS})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)
 
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)
 
include_directories(  ${GLUT_INCLUDE_DIR}  )
 
if(APPLE)
  include_directories(${OPENGL_INCLUDE_DIR})
else()
find_package(Glew REQUIRED)
if(NOT GLEW_FOUND)
    message(ERROR " GLEW not found!")
endif(NOT GLEW_FOUND)

  include_directories(${OPENGL_INCLUDE_DIR})
	include_directories( ${GLEW_INCLUDE_DIR})
	target_link_libraries(camera ${GLEW_LIBRARY} )
	target_link_libraries(bump ${GLEW_LIBRARY} )
endif()

target_link_libraries(bump ${OPENGL_LIBRARIES} )
target_link_libraries(bump ${GLUT_LIBRARY}  )

target_link_libraries(camera ${OPENGL_LIBRARIES}  )
target_link_libraries(camera ${GLUT_LIBRARY}  )


target_link_libraries (bump ${LIBBMATH_LIBRARIES})

target_link_libraries (bump ${PNG_LIBRARY})
target_link_libraries (bump ${SDL_LIBRARY})
target_link_libraries (camera ${PNG_LIBRARY})
target_link_libraries (camera ${SDL_LIBRARY})
target_link_libraries (bump ${SDLIMAGE_LIBRARY})
target_link_libraries (camera ${SDLIMAGE_LIBRARY})
