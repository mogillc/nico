cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if(APPLE)
	set(CMAKE_MACOSX_RPATH 1)
endif()

project (libmogi)

set( LIBNAME mogi )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# options
option( BUILD_FOR_IOS "Build library for iOS" OFF)
option( BUILD_SIMULATION "Build the simulation engine" OFF)

if(BUILD_FOR_IOS)
	add_definitions(-DBUILD_FOR_IOS)
	set( BUILD_WITH_JSONCPP NO )
	set( BUILD_WITH_FTDI OFF )
	set( BUILD_TESTS OFF )
	set( GCOV_ENABLED OFF )
	set( BUILD_SHARED_LIBS OFF )
else()
	set( BUILD_WITH_JSONCPP YES )
	option( BUILD_WITH_FTDI "Build with FTDI support library" ON)
	option( BUILD_TESTS "Build unit test programs" OFF )
	option( GCOV_ENABLED "Use Code Coverage (gcov/lcov)" OFF )
	option( BUILD_SHARED_LIBS "Build the shared library" ON)
endif()
option( BUILD_STATIC_LIBS "Build the static library" ON)

# The version number
set (MAJOR_VERSION 0)
set (MINOR_VERSION 1)
set (MICRO_VERSION 1)
set (VERSION_STRING ${MAJOR_VERSION}.${MINOR_VERSION}.${MICRO_VERSION})

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

if( BUILD_TESTS )
	enable_testing()
endif( BUILD_TESTS )

if( GCOV_ENABLED )
	enable_testing()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fprofile-arcs -ftest-coverage -fno-elide-constructors -Wno-invalid-offsetof")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -fprofile-arcs -ftest-coverage -fno-elide-constructors -Wno-invalid-offsetof")
	set(CMAKE_BUILD_TYPE     Debug)
	include( CodeCoverage )
endif( GCOV_ENABLED )

#add_definitions(-Wall -Wextra)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# CMake
#if("${CMAKE_BUILD_TYPE}" STREQUAL "")
#   set(CMAKE_BUILD_TYPE     Debug)
#else("${CMAKE_BUILD_TYPE}" STREQUAL "")
#   set(CMAKE_BUILD_TYPE     "")
#endif("${CMAKE_BUILD_TYPE}" STREQUAL "")

set(CMAKE_COLOR_MAKEFILE ON)

# Debug build
#message("-- Build type: ${CMAKE_BUILD_TYPE}")
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
   add_definitions(-DDEBUG)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

MACRO(HEADER_DIRECTORIES return_list)
	FILE(GLOB_RECURSE new_list *.h)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()
#file(GLOB_RECURSE ALL_HEADER_DIRS "*/include")
HEADER_DIRECTORIES(ALL_HEADER_DIRS)
include_directories(${ALL_HEADER_DIRS})

include( MogiLibrary )

# FIND PACKAGES:
#	Threads:
if( BUILD_FOR_IOS )
	add_definitions(-DTHREADS_FOUND)
else()
	find_package (Threads REQUIRED)
	if(THREADS_FOUND)
		add_definitions(-DTHREADS_FOUND)
	endif()
endif()

# JsonCPP:
if(BUILD_WITH_JSONCPP)
	find_package(LibJsoncpp)
	if(LIBJSONCPP_FOUND)
		include_directories(${LIBJSONCPP_INCLUDE_DIRS})
		set( DEPENDENCIES ${DEPENDENCIES} ${LIBJSONCPP_LIBRARIES} )
	endif()
endif()

#	Include libFTDI:
if(BUILD_WITH_FTDI)
	find_package (LibFtdi)
	if(LIBFTDI_FOUND)
		add_definitions(-DLIBFTDI_FOUND)
		include_directories(${LIBFTDI_INCLUDE_DIRS})
		set( DEPENDENCIES ${DEPENDENCIES} ${LIBFTDI_LIBRARIES} )
		#message("LIBFTDI_LIBRARY = ${LIBFTDI_LIBRARY}")
	else()
		message("Warning: FTDI libraries not found!")
	endif()
endif()

# SET COMPONENTS:
INCLUDE(CPack)
set(CPACK_COMPONENTS_ALL headers)

set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "C++ Headers")
set(CPACK_COMPONENT_HEADERS_DESCRIPTION "C/C++ header files.")
set(CPACK_COMPONENT_HEADERS_GROUP    "Development")

if(BUILD_STATIC_LIBS)
	set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} staticlibs)
	set(CPACK_COMPONENT_STATICLIBS_DISPLAY_NAME "Static libraries")
	set(CPACK_COMPONENT_STATICLIBS_DESCRIPTION "Static library, good if you want to embed libmogi in your application.")
	set(CPACK_COMPONENT_STATICLIBS_GROUP "Development")
endif(BUILD_STATIC_LIBS)

if(BUILD_SHARED_LIBS)
	set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} sharedlibs)
	set(CPACK_COMPONENT_SHAREDLIBS_DISPLAY_NAME "Shared libraries")
	set(CPACK_COMPONENT_SHAREDLIBS_DESCRIPTION "Shared library for general use.")
	set(CPACK_COMPONENT_SHAREDLIBS_GROUP "Development")
endif(BUILD_SHARED_LIBS)

#if(OPENGL_FOUND)
	set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} resources)
#endif(OPENGL_FOUND)



# Create suffix to eventually install in lib64
IF(CMAKE_SIZEOF_VOID_P EQUAL 4)
    SET(LIB_SUFFIX "")
    SET(PACK_ARCH "")
  ELSE(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(LIB_SUFFIX 64)
    SET(PACK_ARCH .x86_64)
endif(CMAKE_SIZEOF_VOID_P EQUAL 4)

# Package information
set(CPACK_PACKAGE_VERSION              ${VERSION_STRING})
set(CPACK_PACKAGE_CONTACT              "Matt Bunting <matt@mogi.io>")
set(CPACK_PACKAGE_DESCRIPTION          "libmogi library.")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY  ${CPACK_PACKAGE_DESCRIPTION}
                                       )

# Package settings
if(${UNIX})
  set(CPACK_GENERATOR                    "DEB;RPM")
  set(CPACK_CMAKE_GENERATOR              "Unix Makefiles")
  set(CPACK_PACKAGE_NAME                 ${PROJECT_NAME})
  set(CPACK_PACKAGE_FILE_NAME            ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}${PACK_ARCH})
endif(${UNIX})

if(${WIN32})
  set(CPACK_GENERATOR                    "NSIS")
  set(CPACK_CMAKE_GENERATOR              "MinGW Makefiles")
  set(CPACK_PACKAGE_NAME                 "${PROJECT_NAME}")
  set(CPACK_PACKAGE_VENDOR               "")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY    "libmogi")
  set(CPACK_PACKAGE_FILE_NAME            "${PROJECT_NAME}-${VERSION_STRING}-win32")
  set(CPACK_NSIS_DISPLAY_NAME            "libmogi")
  set(CPACK_NSIS_MODIFY_PATH             "ON")
endif(${WIN32})

set(CPACK_RESOURCE_FILE_LICENSE        ${CMAKE_SOURCE_DIR}/LICENSE)

set(CPACK_SOURCE_GENERATOR             TGZ)
set(CPACK_SOURCE_IGNORE_FILES          "\\\\.git")
set(CPACK_SOURCE_PACKAGE_FILE_NAME     ${CPACK_PACKAGE_FILE_NAME})

# Subdirectories
if(${UNIX})
  set(CPACK_SET_DESTDIR "ON")
endif(${UNIX})




if(BUILD_SIMULATION)
	add_subdirectory( simulation )	
	#set( DEPENDENCIES ${DEPENDENCIES} mogisimulation )
endif(BUILD_SIMULATION)





new_mogi_library( ${LIBNAME} ${DEPENDENCIES} )




# PkgConfig
#set(prefix      ${CMAKE_INSTALL_PREFIX})
#set(exec_prefix ${CMAKE_INSTALL_PREFIX}/bin)
#set(includedir  ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})
#
#if(${UNIX})
#   set(libdir      ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
#endif(${UNIX})
#if(${WIN32})
#   set(libdir      ${CMAKE_INSTALL_PREFIX}/bin)
#endif(${WIN32})

#configure_file(${CMAKE_SOURCE_DIR}/libftdi.pc.in ${CMAKE_BINARY_DIR}/libftdi.pc @ONLY)
#configure_file(${CMAKE_SOURCE_DIR}/libftdipp.pc.in ${CMAKE_BINARY_DIR}/libftdipp.pc @ONLY)
#install(FILES ${CMAKE_BINARY_DIR}/libftdi.pc ${CMAKE_BINARY_DIR}/libftdipp.pc        DESTINATION lib${LIB_SUFFIX}/pkgconfig)



#message("-- Install Directory: ${CMAKE_INSTALL_PREFIX}")






