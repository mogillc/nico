cmake_minimum_required (VERSION 2.8)

project (hexapodGL)
set(CMAKE_BUILD_TYPE Debug)

# The version number
set (hexapodGL_VERSION_MAJOR 0)
set (hexapodGL_VERSION_MINOR 0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../../libmogi/cmake/Modules/")

file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/STL
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/Objects
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/Pics
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

#set(c_sources src/models.cpp)
FILE( GLOB SRCS src/*.h src/*.cpp )

#	Find PNG:
#find_package (LibPng REQUIRED)
#include_directories(${PNG_INCLUDE_DIR})
#set(LIBS ${LIBS} ${PNG_LIBRARY})

#	Find Assimp:
find_package (LibAssimp REQUIRED)
include_directories(${ASSIMP_INCLUDE_DIR})
set(LIBS ${LIBS} ${ASSIMP_LIBRARY})

#	Find SDL2:
find_package (LibSdl2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
set(LIBS ${LIBS} ${SDL2_LIBRARY})

#	Find Mogi:
find_package (LibMogi REQUIRED)
include_directories(${LIBMOGI_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LIBMOGI_LIBRARIES})

#	Find MBsimulation:
find_package (LibMBsimulation REQUIRED)
include_directories(${LIBMBSIMULATION_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LIBMBSIMULATION_LIBRARIES})

find_package(OpenGL REQUIRED)
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})

find_package(GLUT )

if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)
 
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
else()
	include_directories(  ${GLUT_INCLUDE_DIR}  )

endif(NOT GLUT_FOUND)
 

message("Include for GLUT:    ${GLUT_INCLUDE_DIR}") 
message("Include for GLEW:    ${GLEW_INCLUDE_DIR}")
message("Include for OpenGL:  ${OPENGL_INCLUDE_DIR}")

include_directories(${OPENGL_INCLUDE_DIR})
if(APPLE)
  include_directories(${OPENGL_INCLUDE_DIR})
  include_directories(${PNG_INCLUDE_DIR})
message("PNG headers: ${PNG_INCLUDE_DIR}")
else()
	find_package(Glew REQUIRED)
	if(NOT GLEW_FOUND)
		message(ERROR " GLEW not found!")
	endif(NOT GLEW_FOUND)

	#GLEW libraries
	#	add_library(glew_static STATIC IMPORTED)
	#	set_target_properties(glew_static PROPERTIES
	#		IMPORTED_LOCATION )
	#	target_link_libraries(hexapodGL glew_static)
 
	include_directories(${OPENGL_INCLUDE_DIR})
	include_directories( ${GLEW_INCLUDE_DIR})

	# COMMENTED BY ADRIAN
	#target_link_libraries(hexapodGL ${GLEW_LIBRARY} )
	#target_link_libraries(armGL ${GLEW_LIBRARY} )
	#target_link_libraries(simple ${GLEW_LIBRARY} )
endif()

add_library(GLcommon src/Arm6DOF.h src/Arm6DOF.cpp src/calibration.h src/common.h src/common.cpp)
set(LIBS ${LIBS} GLcommon)

add_executable(hexapodGL src/hexapodGL.cpp )
add_executable(armGL src/arm.cpp )
add_executable(simple src/simple.cpp)

target_link_libraries(hexapodGL ${LIBS})
target_link_libraries(armGL ${LIBS})
target_link_libraries(simple ${LIBS})

if (NOT APPLE) #ADDED BY ADRIAN
	target_link_libraries(hexapodGL ${GLEW_LIBRARY} )
	target_link_libraries(armGL ${GLEW_LIBRARY} )
	target_link_libraries(simple ${GLEW_LIBRARY} )
endif()
