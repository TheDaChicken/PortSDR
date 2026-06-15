# - Try to find the libairspy library
# Once done this defines
#
#  AIRSPYHF_FOUND - system has libairspy
#  AIRSPYHF_INCLUDE_DIR - the libairspy include directory
#  AIRSPYHF_LIBRARIES - Link these to use libairspy

include(FindPackageHandleStandardArgs)

if (NOT WIN32)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    find_package(PkgConfig)
    pkg_check_modules(PC_AIRSPYHF QUIET AIRSPYHF)
endif ()

if (NOT LibUSB_FOUND)
    find_package(LibUSB REQUIRED)
endif ()

FIND_PATH(AIRSPYHF_INCLUDE_DIR
        NAMES airspy.h
        HINTS $ENV{AIRSPYHF_DIR}/include ${PC_AIRSPYHF_INCLUDEDIR}
        PATHS /usr/local/include/AIRSPYHF /usr/include/AIRSPYHF /usr/local/include
        /usr/include ${CMAKE_SOURCE_DIR}/../AIRSPYHF/src /opt/local/include/AIRSPYHF
)

FIND_LIBRARY(AIRSPYHF_LIBRARIES
        NAMES airspyhf
        HINTS $ENV{AIRSPYHF_DIR}/lib ${PC_AIRSPYHF_LIBDIR}
        PATHS /usr/local/lib /usr/lib /opt/local/lib ${PC_AIRSPYHF_LIBDIR} ${PC_AIRSPYHF_LIBRARY_DIRS} ${CMAKE_SOURCE_DIR}/../AIRSPYHFhf/src
)

find_package_handle_standard_args(AIRSPYHF DEFAULT_MSG AIRSPYHF_LIBRARIES AIRSPYHF_INCLUDE_DIR)
mark_as_advanced(AIRSPYHF_INCLUDE_DIR AIRSPYHF_LIBRARIES)

if (AIRSPYHF_FOUND AND NOT DEFINED AIRSPYHF::AIRSPYHF)
    add_library(AIRSPYHF::AIRSPYHF INTERFACE IMPORTED)
    set_target_properties(AIRSPYHF::AIRSPYHF PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${AIRSPYHF_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${AIRSPYHF_LIBRARIES}"
    )
    target_link_libraries(AIRSPYHF::AIRSPYHF INTERFACE libusb::libusb)
endif ()