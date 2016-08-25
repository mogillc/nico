cmake_minimum_required (VERSION 2.6)

project (androidAppInterface)

# The version number
set (appInterfaceDemo_VERSION_MAJOR 0)
set (appInterfaceDemo_VERSION_MINOR 0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../../libmogi/cmake/Modules/")

include_directories(include)
add_executable(androidAppInterface 
				src/androidAppInterface.cpp 
				src/ApplicationServer.cpp 
				src/ApplicationCommunicator.cpp 
				src/HexapodPayload.cpp )


#	Find Mogi:
find_package (LibMogi REQUIRED)
include_directories(${LIBMOGI_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LIBMOGI_Libraries})

target_link_libraries (androidAppInterface ${LIBMOGI_LIBRARIES})
