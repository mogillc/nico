# - Try to find LibHexapod
# Once done this will define
#  LIBHEXAPOD_FOUND - System has LibXml2
#  LIBHEXAPOD_INCLUDE_DIRS - The LibXml2 include directories
#  LIBHEXAPOD_LIBRARIES - The libraries needed to use LibXml2
#  LIBHEXAPOD_DEFINITIONS - Compiler switches required for using LibXml2

find_package(PkgConfig)
pkg_check_modules(PC_LIBFTDI QUIET libftdi)
set(LIBFTDI_DEFINITIONS ${PC_LIBFTDI_CFLAGS_OTHER})

find_path(LIBFTDI_INCLUDE_DIR ftdi.h
          HINTS /usr/local/include/ ${PC_LIBFTDI_INCLUDEDIR} ${PC_LIBFTDI_INCLUDE_DIRS}
          PATH_SUFFIXES libftdi )

find_library(LIBFTDI_LIBRARY NAMES ftdi libftdi
             HINTS ${PC_LIBFTDI_LIBDIR} ${PC_LIBFTDI_LIBRARY_DIRS} )

set(LIBFTDI_LIBRARIES ${LIBFTDI_LIBRARY} )
set(LIBFTDI_INCLUDE_DIRS ${LIBFTDI_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibFtdi  DEFAULT_MSG
                                  LIBFTDI_LIBRARY LIBFTDI_INCLUDE_DIR)

mark_as_advanced(LIBFTDI_INCLUDE_DIR LIBFTDI_LIBRARY )
