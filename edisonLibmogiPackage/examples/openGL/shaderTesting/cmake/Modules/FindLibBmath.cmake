# - Try to find LibBMATH
# Once done this will define
#  LIBBMATH_FOUND - System has LibXml2
#  LIBBMATH_INCLUDE_DIRS - The LibXml2 include directories
#  LIBBMATH_LIBRARIES - The libraries needed to use LibXml2
#  LIBBMATH_DEFINITIONS - Compiler switches required for using LibXml2

#find_package(PkgConfig)
#pkg_check_modules(PC_LIBBMATH libbmath)
#set(LIBBMATH_DEFINITIONS ${PC_LIBBMATH_CFLAGS_OTHER})

find_path(LIBBMATH_INCLUDE_DIR bmath.h
          HINTS ${PC_LIBBMATH_INCLUDEDIR} ${PC_LIBBMATH_INCLUDE_DIRS}
          PATH_SUFFIXES libbmath )

find_library(LIBBMATH_LIBRARY NAMES bmath libbmath
             HINTS ${PC_LIBBMATH_LIBDIR} ${PC_LIBBMATH_LIBRARY_DIRS} /usr/local/lib64)

set(LIBBMATH_LIBRARIES ${LIBBMATH_LIBRARY} )
set(LIBBMATH_INCLUDE_DIRS ${LIBBMATH_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibBmath  DEFAULT_MSG
                                  LIBBMATH_LIBRARY LIBBMATH_INCLUDE_DIR)

mark_as_advanced(LIBBMATH_INCLUDE_DIR LIBBMATH_LIBRARY )

#if(${LIBBMATH_FOUND})
#	message("Found bmath library!")
#	message("-- bmath.h: ${LIBBMATH_INCLUDE_DIR}")
#	message("-- libbmath: ${LIBBMATH_LIBRARY}")
#endif(${LIBBMATH_FOUND})
