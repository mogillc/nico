# - Try to find LibHexapod
# Once done this will define
#  LIBHEXAPOD_FOUND - System has LibXml2
#  LIBHEXAPOD_INCLUDE_DIRS - The LibXml2 include directories
#  LIBHEXAPOD_LIBRARIES - The libraries needed to use LibXml2
#  LIBHEXAPOD_DEFINITIONS - Compiler switches required for using LibXml2

#find_package(PkgConfig)
#pkg_check_modules(PC_LIBHEXAPOD libmogi)
#set(LIBHEXAPOD_DEFINITIONS ${PC_LIBHEXAPOD_CFLAGS_OTHER})

find_path(LIBHEXAPOD_INCLUDE_DIR hexapod.h
          HINTS ${PC_LIBHEXAPOD_INCLUDEDIR} ${PC_LIBHEXAPOD_INCLUDE_DIRS}
          PATH_SUFFIXES libmogi )

find_library(LIBHEXAPOD_LIBRARY NAMES hexapod libmogi
             HINTS ${PC_LIBHEXAPOD_LIBDIR} ${PC_LIBHEXAPOD_LIBRARY_DIRS} /usr/local/lib64)

set(LIBHEXAPOD_LIBRARIES ${LIBHEXAPOD_LIBRARY} )
set(LIBHEXAPOD_INCLUDE_DIRS ${LIBHEXAPOD_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibHexapod  DEFAULT_MSG
                                  LIBHEXAPOD_LIBRARY LIBHEXAPOD_INCLUDE_DIR)

mark_as_advanced(LIBHEXAPOD_INCLUDE_DIR LIBHEXAPOD_LIBRARY )

#if(${LIBHEXAPOD_FOUND})
#	message("Found hexapod library!")
#	message("-- hexapod.h: ${LIBHEXAPOD_INCLUDE_DIR}")
#	message("-- libmogi: ${LIBHEXAPOD_LIBRARY}")
#endif(${LIBHEXAPOD_FOUND})
