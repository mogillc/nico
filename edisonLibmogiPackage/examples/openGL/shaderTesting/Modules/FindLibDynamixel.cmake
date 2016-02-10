# - Try to find LibDynamixel
# Once done this will define
#  LIBDYNAMIXEL_FOUND - System has LibXml2
#  LIBDYNAMIXEL_INCLUDE_DIRS - The LibXml2 include directories
#  LIBDYNAMIXEL_LIBRARIES - The libraries needed to use LibXml2
#  LIBDYNAMIXEL_DEFINITIONS - Compiler switches required for using LibXml2

#find_package(PkgConfig)
#pkg_check_modules(PC_LIBDYNAMIXEL libdynamixel)
#set(LIBDYNAMIXEL_DEFINITIONS ${PC_LIBDYNAMIXEL_CFLAGS_OTHER})

find_path(LIBDYNAMIXEL_INCLUDE_DIR dynamixel.h
          HINTS ${PC_LIBDYNAMIXEL_INCLUDEDIR} ${PC_LIBDYNAMIXEL_INCLUDE_DIRS}
          PATH_SUFFIXES libdynamixel )

find_library(LIBDYNAMIXEL_LIBRARY NAMES dynamixel libdynamixel
             HINTS ${PC_LIBDYNAMIXEL_LIBDIR} ${PC_LIBDYNAMIXEL_LIBRARY_DIRS} /usr/local/lib64)

set(LIBDYNAMIXEL_LIBRARIES ${LIBDYNAMIXEL_LIBRARY} )
set(LIBDYNAMIXEL_INCLUDE_DIRS ${LIBDYNAMIXEL_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibDynamixel  DEFAULT_MSG
                                  LIBDYNAMIXEL_LIBRARY LIBDYNAMIXEL_INCLUDE_DIR)

mark_as_advanced(LIBDYNAMIXEL_INCLUDE_DIR LIBDYNAMIXEL_LIBRARY )

#if(${LIBDYNAMIXEL_FOUND})
#	message("Found dynamixel library!")
#	message("-- dynamixel.h: ${LIBDYNAMIXEL_INCLUDE_DIR}")
#	message("-- libdynamixel: ${LIBDYNAMIXEL_LIBRARY}")
#endif(${LIBDYNAMIXEL_FOUND})
