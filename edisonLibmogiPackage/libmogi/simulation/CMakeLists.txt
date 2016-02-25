set ( DEPENDENCIES mogi )

#Needed for OSX/ Linux simulation
if(BUILD_FOR_IOS OR ANDROID)
	find_package(OpenGLES)
	if(OPENGLES_FOUND)
		add_definitions(-DOPENGL_EXISTS)
		add_definitions(-DOPENGLES_FOUND)
		include_directories(${OPENGLES_INCLUDE_DIR})
		set(DEPENDENCIES ${DEPENDENCIES} ${OPENGLES_LIBRARIES})
		set( OPENGL_FOUND YES )
	else()
		message("not able to find OPENGLES_FOUND=no :(:(")
	endif()
else()
	find_package(OpenGL)
	if(OPENGL_FOUND)
		include_directories(${OPENGL_INCLUDE_DIR})
		set(DEPENDENCIES ${DEPENDENCIES} ${OPENGL_LIBRARIES})
		add_definitions(-DOPENGL_EXISTS)
	else()
	    message(WARNING " OPENGL not found!")
	endif()
endif(BUILD_FOR_IOS OR ANDROID)



if(OPENGL_FOUND)
	option( BUILD_WITH_SDL2 "Build with SDL 2" ON)
	option( BUILD_WITH_PNG "Build with libpng" OFF)

	add_definitions(-DOPENGL_EXISTS)
	#	Find PNG:
	#	This is used for....?
	if(BUILD_WITH_PNG)
		find_package (LibPng REQUIRED)
		if(PNG_FOUND)
			add_definitions(-DPNG_FOUND)
			include_directories(${PNG_INCLUDE_DIR})
			#message("PNG_INCLUDE_DIR=${PNG_INCLUDE_DIR}")	
			set( DEPENDENCIES ${DEPENDENCIES} ${PNG_LIBRARY} )		
		endif()
	endif()

	#	Find Assimp:
	#	Used heavily, in almost every file.
	if(BUILD_FOR_IOS)
		add_definitions(-DASSIMP_FOUND)	# TODO: fix hardcoding
		include_directories(/Users/matt/Downloads/assimp-master/include)	# TODO: fix hardcoding
		set( DEPENDENCIES ${DEPENDENCIES} ${CMAKE_SOURCE_DIR}/libassimp-fat.a ${CMAKE_SOURCE_DIR}/libzlibstatic.a )	# TODO: fix hardcoding
	elseif(ANDROID)
		# zlib from android ndk
		find_package(ZLIB REQUIRED)
		include_directories(${ZLIB_INCLUDE_DIR})
		set( DEPENDENCIES ${DEPENDENCIES} ${ZLIB_LIBRARIES})

		add_definitions(-DASSIMP_FOUND)	
		include_directories(/home/adrian/assimp/include)	# TODO: fix hardcoding

		if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64") # TODO: fix hardcoding
			set( DEPENDENCIES ${DEPENDENCIES} ${CMAKE_SOURCE_DIR}/port/android/lib-assimp/arm64-v8a/code/libassimp.a )
		elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "armv7-a") # TODO: fix hardcoding
			set( DEPENDENCIES ${DEPENDENCIES} ${CMAKE_SOURCE_DIR}/port/android/lib-assimp/armeabi-v7a/code/libassimp.a )
		else()
			#SHIT we don't support this architecture
			message("Libmogi does not yet support this Android ABI: " ${CMAKE_SYSTEM_PROCESSOR})
			set( DEPENDENCIES ${DEPENDENCIES} ${CMAKE_SOURCE_DIR}/port/android/lib-assimp/armeabi-v7a/code/libassimp.a )
		endif()		
	else(BUILD_FOR_IOS)
		find_package (LibAssimp)
		if(ASSIMP_FOUND)
			include_directories(${ASSIMP_INCLUDE_DIR})	
			set( DEPENDENCIES ${DEPENDENCIES} ${ASSIMP_LIBRARY} )
		endif()
	endif(BUILD_FOR_IOS)

	#	Find SDL:
	#	Using SDL2 instead
	#find_package (LibSdl REQUIRED)
	#include_directories(${SDL_INCLUDE_DIR})

	#	Find SDL2:
	#	Required by Texture.cpp for texture loading.
	if(BUILD_WITH_SDL2)
		find_package (LibSdl2 REQUIRED)
		if(SDL2_FOUND)
			include_directories(${SDL2_INCLUDE_DIR})
			#message("SDL2_INCLUDE_DIR=${SDL2_INCLUDE_DIR}")
			set( DEPENDENCIES ${DEPENDENCIES} ${SDL2_LIBRARY} )
		endif()
	endif()

	find_package( GLUT )

	#message("Include for GLUT:    ${GLUT_INCLUDE_DIR}") 
	#message("Include for GLEW:    ${GLEW_INCLUDE_DIR}")
	#message("Include for OpenGL:  ${OPENGL_INCLUDE_DIR}")

	if(APPLE OR ANDROID)
		#  include_directories(${OPENGL_INCLUDE_DIR}/Headers)

	else()
		find_package(Glew REQUIRED)
		if(NOT GLEW_FOUND)
		    #message(ERROR " GLEW not found!")
		endif(NOT GLEW_FOUND)

		include_directories(${OPENGL_INCLUDE_DIR})
  		include_directories( ${GLEW_INCLUDE_DIR})
		set( DEPENDENCIES ${DEPENDENCIES} ${GLEW_LIBRARY} )
	endif()	

	if(NOT GLUT_FOUND)
	    #message(ERROR " GLUT not found!")
	else()
		include_directories(  ${GLUT_INCLUDE_DIR}  )
		set( DEPENDENCIES ${DEPENDENCIES} ${GLUT_LIBRARY} )
	endif(NOT GLUT_FOUND)

	set( LIBNAME mbsimulation )
	new_mogi_library( ${LIBNAME} ${DEPENDENCIES} )

endif(OPENGL_FOUND)


