
# Given a path, create a test:
function( mogi_create_test TEST_SOURCE )
	get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE )
	#message( "Creating unit tests named ${TEST_NAME}" )
	project (${TEST_NAME} )
	add_executable( ${TEST_NAME} ${TEST_NAME}.cpp)
	target_link_libraries ( ${TEST_NAME} ${ARGN} )
	add_test( NAME ${TEST_NAME} COMMAND ${TEST_NAME} )
	if( GCOV_ENABLED )
		SETUP_TARGET_FOR_COVERAGE(
				${TEST_NAME}_coverage	# Name for custom target.
				${TEST_NAME}	# Name of the test driver executable that runs the tests.
						# NOTE! This should always have a ZERO as exit code
						# otherwise the coverage generation will not complete.
				coverage	# Name of output directory.
				)
	endif( GCOV_ENABLED )
endfunction( mogi_create_test )

function( mogi_add_tests )
	file( GLOB TEST_SOURCES *.cpp )

	# Make sure there are tests:
	set( LIBS ${ARGN} )
	list( LENGTH TEST_SOURCES TEST_SOURCES_LENGTH)
	if(TEST_SOURCES_LENGTH)
		# Create a test for each source file:
		foreach( TEST_SOURCE ${TEST_SOURCES} )
		#	message(" - TEST_SOURCE=${TEST_SOURCE}  LIBS=${LIBS}" )
			mogi_create_test( ${TEST_SOURCE} ${LIBS} )
		endforeach()
	endif(TEST_SOURCES_LENGTH)
endfunction()

function( new_mogi_library LIBNAME )
	# Everything below this comment is used to install the library in 
	# the current directory.  LIBNAME is used to define the library name,
	# DEPENDENCIES is to define library dependencies.  
	# This is based on the directory structure:
	#
	# - src
	# - include
	#   |- mogi_version_i.h.in (optional)
	# - install     (optional)
	# - resources   (optional)
	# - tests       (optional)
	
	set( DEPENDENCIES ${ARGN} )
	#message( " - creating library named ${LIBNAME} dependencies:${DEPENDENCIES}")
	
	# Versioning header generation
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/mogi_version_i.h.in")
		include_directories( ${CMAKE_CURRENT_BINARY_DIR}
		                     ${CMAKE_CURRENT_SOURCE_DIR}
			                     )
		
		# Version information
		set(SNAPSHOT_VERSION "unknown")
		execute_process(COMMAND svnversion ..
		                OUTPUT_VARIABLE GIT_DESCRIBE_OUTPUT
		                RESULT_VARIABLE GIT_DESCRIBE_RESULT
		                OUTPUT_STRIP_TRAILING_WHITESPACE
		                )
		if(${GIT_DESCRIBE_RESULT} STREQUAL 0)
			set(SVN_VERSION ${GIT_DESCRIBE_OUTPUT})
			message(STATUS "Detected svn version: ${SVN_VERSION}")
		endif ()

		if(BUILD_FOR_IOS)
			set(PORT_HEADERS "\#include \"resourceInterface.h\"")
		else()
			set(PORT_HEADERS "")
		endif()
		
		configure_file(include/mogi_version_i.h.in "${CMAKE_CURRENT_BINARY_DIR}/mogi_version_i.h" @ONLY)
	endif()
	
	# Targets
	file(GLOB_RECURSE ${LIBNAME}_SOURCES "src/*.cpp")
	file(GLOB_RECURSE ${LIBNAME}_HEADERS "include/*.h")
	if( BUILD_FOR_IOS )
		set( ${LIBNAME}_SOURCES ${${LIBNAME}_SOURCES} 
								${CMAKE_SOURCE_DIR}/port/ios/resourceObject.mm )
		set( ${LIBNAME}_HEADERS ${${LIBNAME}_HEADERS} 
								${CMAKE_SOURCE_DIR}/port/ios/resourceObject.h
								${CMAKE_SOURCE_DIR}/port/ios/resourceInterface.h )
		#message( "${LIBNAME}_SOURCES=${${LIBNAME}_SOURCES}")
	endif()
	set(c_sources     ${${LIBNAME}_SOURCES} ${${LIBNAME}_HEADERS})
	set(c_headers     ${${LIBNAME}_HEADERS})

	add_library( ${LIBNAME} SHARED ${c_sources})
	include_directories ( include )
	
	math(EXPR VERSION_FIXUP "${MAJOR_VERSION} + 1")    # Compatibility with previous releases
	set_target_properties(${LIBNAME} PROPERTIES VERSION ${VERSION_FIXUP}.${MINOR_VERSION}.${MICRO_VERSION} SOVERSION 1)	

	# Static library
	add_library(${LIBNAME}-static STATIC ${c_sources})
	set_target_properties(${LIBNAME}-static PROPERTIES OUTPUT_NAME "${LIBNAME}")
	
	# Prevent clobbering each other during the build
	set_target_properties(${LIBNAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	set_target_properties(${LIBNAME}-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	
	# Dependencies
	target_link_libraries (${LIBNAME} ${DEPENDENCIES})
	
	if( NOT BUILD_FOR_IOS )
		# Resources
		if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
			file(GLOB ${LIBNAME}_RESOURCES resources/*)
			set(${LIBNAME}_resources ${${LIBNAME}_RESOURCES})
			set(resources_subdirectory etc/${LIBNAME} PARENT_SCOPE)
			set(resources_subdirectory etc/${LIBNAME})
			file(COPY
		    			${CMAKE_CURRENT_SOURCE_DIR}/resources
		    			DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
						)	
			install( DIRECTORY ${${LIBNAME}_resources}
		           		DESTINATION ${resources_subdirectory}
						COMPONENT resources
		           		)
				add_definitions(-DRESOURCES_DIRECTORY="${CMAKE_INSTALL_PREFIX}/${resources_subdirectory}")
		endif()
	endif( NOT BUILD_FOR_IOS )
	


		# Install
		if(${UNIX})
			if(BUILD_SHARED_LIBS)
				install( TARGETS ${LIBNAME}
					LIBRARY DESTINATION lib #lib${LIB_SUFFIX}
					COMPONENT sharedlibs
					)
			endif(BUILD_SHARED_LIBS)
		
			if(BUILD_STATIC_LIBS)
				install( TARGETS ${LIBNAME}-static
					ARCHIVE DESTINATION lib #lib${LIB_SUFFIX}
					COMPONENT staticlibs
					)
			endif(BUILD_STATIC_LIBS)
		
			install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ #${c_headers}
		            DESTINATION include
		            COMPONENT headers
		            )
		
			#message("-- Install Directory ${LIBNAME}: lib${LIB_SUFFIX}")
			#message("-- Install Directory ${LIBNAME}-static: lib${LIB_SUFFIX}")
			#message("-- Install Directory ${c_headers}: include/${LIB_SUFFIX}")
			#message("-- Resource Directory ${${LIBNAME}_resources}: ${resources_subdirectory}")
			#message("-- #define RESOURCES_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${resources_subdirectory}")
		endif(${UNIX})

if (BUILD_FOR_IOS)	# hack since there is a discrepancy between the output to Debug and output to Debug-iosphone
   install (FILES ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/libmogi.1.1.1.dylib
			DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION} )
endif (BUILD_FOR_IOS)
		
		if(${WIN32})
			if(BUILD_SHARED_LIBS)
				install( TARGETS ${LIBNAME}
						DESTINATION bin
						COMPONENT sharedlibs
						)
			endif(BUILD_SHARED_LIBS)
		
			if(BUILD_STATIC_LIBS)
				install( TARGETS ${LIBNAME}-static
						DESTINATION bin
						COMPONENT staticlibs
						)
			endif(BUILD_STATIC_LIBS)
		
			install( FILES ${c_headers}
					DESTINATION include
					COMPONENT headers
					)
		
		endif(${WIN32})

	if( NOT BUILD_FOR_IOS )
		# Tests
		if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
			if(BUILD_TESTS)
				add_subdirectory( tests )
			endif(BUILD_TESTS)
		endif()
		
		# Install binaries
		if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/install")
			add_subdirectory( install )
		endif()
	
		foreach(FILE ${c_sources}) 
		    # Get the directory of the source file
		    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
		
		    # Remove common directory prefix to make the group
		    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
		
		    # Make sure we are using windows slashes
		    string(REPLACE "/" "\\" GROUP "${GROUP}")
	
	   	 # Group into "Source Files" and "Header Files"
	   		 if ("${FILE}" MATCHES ".*\\.cpp")
	       set(GROUP "Source Files${GROUP}")
			    elseif("${FILE}" MATCHES ".*\\.h")
		       set(GROUP "Header Files${GROUP}")
		    endif()
		
		    source_group("${GROUP}" FILES "${FILE}")
		endforeach()
	endif( NOT BUILD_FOR_IOS )
	
endfunction()



function( mogi_add_utilities )
	file( GLOB UTILITIES *.cpp )
	foreach( utilityfile ${UTILITIES} )
		# Each utility will be the same name (without extension) as the cpp file.
		get_filename_component(utilityname ${utilityfile} NAME_WE )
	
		#message( "executable named ${utilityname} from file ${utilityfile}" )
		
		add_executable( ${utilityname} ${utilityfile} )
	
		target_link_libraries ( ${utilityname} ${ARGN})
	
		# Install
		if(${UNIX})
			set_target_properties( ${utilityname} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
			install( TARGETS ${utilityname}
	            		DESTINATION bin
	            		)
		endif(${UNIX})	
	endforeach( utilityfile ${UTILITIES} )

endfunction()
