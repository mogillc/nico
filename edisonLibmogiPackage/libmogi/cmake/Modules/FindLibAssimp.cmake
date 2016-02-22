# This module defines
#  ASSIMP_INCLUDE_DIR, where to find assimp header files location, etc.
#  ASSIMP_FOUND, If false, do not try to use ASSIMP.
# also defined, but not for general use are
#  ASSIMP_LIBRARY, where to find the ASSIMP library.

FIND_PATH(
  ASSIMP_INCLUDE_DIR assimp
  PATHS
    ${ASSIMP_ROOT}
  PATH_SUFFIXES 
    include
    include/assimp
  NO_DEFAULT_PATH )

FIND_PATH(
  ASSIMP_INCLUDE_DIR assimp
  PATHS
    ${ASSIMP_ROOT}
  PATH_SUFFIXES 
    include
    include/assimp )

SET(ASSIMP_NAMES ${ASSIMP_NAMES} assimp)

FIND_LIBRARY(
  ASSIMP_LIBRARY 
  NAMES 
    ${ASSIMP_NAMES}
  PATHS
    ${ASSIMP_ROOT}
  PATH_SUFFIXES
    /lib
  NO_DEFAULT_PATHS )

FIND_LIBRARY(
  ASSIMP_LIBRARY 
  NAMES 
    ${ASSIMP_NAMES}
  PATHS
    ${ASSIMP_ROOT}
  PATH_SUFFIXES
    /lib )

# handle the QUIETLY and REQUIRED arguments and set ASSIMP_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ASSIMP DEFAULT_MSG ASSIMP_LIBRARY ASSIMP_INCLUDE_DIR)
if(ASSIMP_FOUND)
	add_definitions(-DASSIMP_FOUND)
endif()
