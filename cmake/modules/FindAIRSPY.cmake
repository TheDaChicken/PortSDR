# - Try to find the AIRSPY library
# Once done this defines
#
#  AIRSPY_FOUND - system has AIRSPY
#  AIRSPY_INCLUDE_DIR - the AIRSPY include directory
#  AIRSPY_LIBRARIES - Link these to use AIRSPY

# Copyright (c) 2013  Benjamin Vernoux
#

include(FindPackageHandleStandardArgs)

if (NOT WIN32)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    find_package(PkgConfig)
    pkg_check_modules(PC_AIRSPY QUIET AIRSPY)
endif ()

if (NOT LibUSB_FOUND)
    find_package(LibUSB REQUIRED)
endif ()

FIND_PATH(AIRSPY_INCLUDE_DIR
        NAMES airspy.h
        HINTS $ENV{AIRSPY_DIR}/include ${PC_AIRSPY_INCLUDEDIR}
        PATHS /usr/local/include/AIRSPY /usr/include/AIRSPY /usr/local/include
        /usr/include /opt/local/include/AIRSPY
)

find_library(AIRSPY_LIBRARIES
        NAMES airspy
        HINTS $ENV{AIRSPY_DIR}/lib ${PC_AIRSPY_LIBDIR}
        PATHS /usr/local/lib /usr/lib /opt/local/lib ${PC_AIRSPY_LIBDIR} ${PC_AIRSPY_LIBRARY_DIRS}
)

find_package_handle_standard_args(AIRSPY DEFAULT_MSG AIRSPY_LIBRARIES AIRSPY_INCLUDE_DIR)
mark_as_advanced(AIRSPY_INCLUDE_DIR AIRSPY_LIBRARIES)

if (AIRSPY_FOUND AND NOT DEFINED AIRSPY::AIRSPY)
    add_library(AIRSPY::AIRSPY INTERFACE IMPORTED)
    set_target_properties(AIRSPY::AIRSPY PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${AIRSPY_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${AIRSPY_LIBRARIES}"
    )
    target_link_libraries(AIRSPY::AIRSPY INTERFACE libusb::libusb)
endif ()

