# - Try to find LibHexapod
# Once done this will define
#  LIBHEXAPOD_FOUND - System has LibXml2
#  LIBHEXAPOD_INCLUDE_DIRS - The LibXml2 include directories
#  LIBHEXAPOD_LIBRARIES - The libraries needed to use LibXml2
#  LIBHEXAPOD_DEFINITIONS - Compiler switches required for using LibXml2

find_package(PkgConfig)
pkg_check_modules(PC_LIBFLYCAPTURE QUIET libflycapture)
set(LIBFLYCAPTURE_DEFINITIONS ${PC_LIBFLYCAPTURE_CFLAGS_OTHER})

find_path(LIBFLYCAPTURE_INCLUDE_DIR FlyCapture2.h
          HINTS /usr/include/ /usr/include/flycapture/ ${PC_LIBFLYCAPTURE_INCLUDEDIR} ${PC_LIBFLYCAPTURE_INCLUDE_DIRS}
          PATH_SUFFIXES libflycapture )

find_library(LIBFLYCAPTURE_LIBRARY NAMES flycapture libflycapture
             HINTS ${PC_LIBFLYCAPTURE_LIBDIR} ${PC_LIBFLYCAPTURE_LIBRARY_DIRS} )

set(LIBFLYCAPTURE_LIBRARIES ${LIBFLYCAPTURE_LIBRARY} )
set(LIBFLYCAPTURE_INCLUDE_DIRS ${LIBFLYCAPTURE_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibFlycapture  DEFAULT_MSG
                                  LIBFLYCAPTURE_LIBRARY LIBFLYCAPTURE_INCLUDE_DIR)

mark_as_advanced(LIBFLYCAPTURE_INCLUDE_DIR LIBFLYCAPTURE_LIBRARY )
