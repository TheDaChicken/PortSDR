# - Try to find the libairspy library
# Once done this defines
#
#  LIBAIRSPY_FOUND - system has libairspy
#  LIBAIRSPY_INCLUDE_DIR - the libairspy include directory
#  LIBAIRSPY_LIBRARIES - Link these to use libairspy


# Copyright (c) 2013  Benjamin Vernoux
#

include(FindPackageHandleStandardArgs)

if (NOT WIN32)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    find_package(PkgConfig)
    pkg_check_modules(PC_LIBAIRSPY QUIET libairspy)
endif ()

if (NOT LIBUSB_FOUND)
    find_package(LIBUSB REQUIRED)
endif ()

FIND_PATH(LIBAIRSPY_INCLUDE_DIR
        NAMES airspy.h
        HINTS $ENV{LIBAIRSPY_DIR}/include ${PC_LIBAIRSPY_INCLUDEDIR}
        PATHS /usr/local/include/libairspy /usr/include/libairspy /usr/local/include
        /usr/include /opt/local/include/libairspy
)

find_library(LIBAIRSPY_LIBRARIES
        NAMES airspy
        HINTS $ENV{LIBAIRSPY_DIR}/lib ${PC_LIBAIRSPY_LIBDIR}
        PATHS /usr/local/lib /usr/lib /opt/local/lib ${PC_LIBAIRSPY_LIBDIR} ${PC_LIBAIRSPY_LIBRARY_DIRS}
)

find_package_handle_standard_args(LIBAIRSPY DEFAULT_MSG LIBAIRSPY_LIBRARIES LIBAIRSPY_INCLUDE_DIR)
mark_as_advanced(LIBAIRSPY_INCLUDE_DIR LIBAIRSPY_LIBRARIES)

if (LIBAIRSPY_FOUND)
    add_library(libairspy::libairspy INTERFACE IMPORTED)
    set_target_properties(libairspy::libairspy PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${LIBAIRSPY_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${LIBAIRSPY_LIBRARIES}"
    )
    target_link_libraries(libairspy::libairspy INTERFACE libusb::libusb)
endif ()

