# This module defines
#  SDL_INCLUDE_DIR, where to find sdl.h, etc.
#  SDL_FOUND, If false, do not try to use sdl.
# also defined, but not for general use are
#  SDL_LIBRARY, where to find the SDL library.

FIND_PATH(
  SDL_INCLUDE_DIR SDL.h
  PATHS
    ${SDL_ROOT}
  PATH_SUFFIXES 
    include/SDL
    include/libsdl
  NO_DEFAULT_PATH )

FIND_PATH(
  SDL_INCLUDE_DIR SDL.h
  PATHS
    ${SDL_ROOT}
  PATH_SUFFIXES 
    include/SDL
    include/libsdl )

SET(SDL_NAMES ${SDL_NAMES} libSDL SDL) 
SET(SDLIMAGE_NAMES libSDL_image SDL_image)
SET(SDLMAIN_NAMES libSDLmain SDLmain)

FIND_LIBRARY(
  SDL_LIBRARY 
  NAMES 
    ${SDL_NAMES}
  PATHS
    ${SDL_ROOT}
  PATH_SUFFIXES
    /lib
  NO_DEFAULT_PATHS )

FIND_LIBRARY(
  SDL_LIBRARY 
  NAMES 
    ${SDL_NAMES}
  PATHS
    ${SDL_ROOT}
  PATH_SUFFIXES
    /lib )

FIND_LIBRARY(
  SDLIMAGE_LIBRARY 
  NAMES 
    ${SDLIMAGE_NAMES}
  PATHS
    ${SDL_ROOT}
  PATH_SUFFIXES
    /lib
  NO_DEFAULT_PATHS )

FIND_LIBRARY(
  SDLIMAGE_LIBRARY 
  NAMES 
    ${SDLIMAGE_NAMES}
  PATHS
    ${SDL_ROOT}
  PATH_SUFFIXES
    /lib )


FIND_LIBRARY(
  SDLMAIN_LIBRARY 
  NAMES 
    ${SDLMAIN_NAMES}
  PATHS
    ${SDL_ROOT}
  PATH_SUFFIXES
    /lib
  NO_DEFAULT_PATHS )

FIND_LIBRARY(
  SDLMAIN_LIBRARY 
  NAMES 
    ${SDLMAIN_NAMES}
  PATHS
    ${SDL_ROOT}
  PATH_SUFFIXES
    /lib )

set(SDL_LIBRARY ${SDL_LIBRARY} ${SDLMAIN_LIBRARY})

# handle the QUIETLY and REQUIRED arguments and set SDL_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL DEFAULT_MSG SDL_LIBRARY SDL_INCLUDE_DIR)
