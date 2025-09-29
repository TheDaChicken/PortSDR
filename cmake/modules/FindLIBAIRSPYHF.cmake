# - Try to find the libairspy library
# Once done this defines
#
#  LIBAIRSPYHF_FOUND - system has libairspy
#  LIBAIRSPYHF_INCLUDE_DIR - the libairspy include directory
#  LIBAIRSPYHF_LIBRARIES - Link these to use libairspy

include(FindPackageHandleStandardArgs)

if (NOT WIN32)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    find_package(PkgConfig)
    pkg_check_modules(PC_LIBAIRSPYHF QUIET libairspyhf)
endif ()

FIND_PATH(LIBAIRSPYHF_INCLUDE_DIR
        NAMES airspy.h
        HINTS $ENV{LIBAIRSPYHF_DIR}/include ${PC_LIBAIRSPYHF_INCLUDEDIR}
        PATHS /usr/local/include/libairspyhf /usr/include/libairspyhf /usr/local/include
        /usr/include ${CMAKE_SOURCE_DIR}/../libairspyhf/src /opt/local/include/libairspyhf
)

FIND_LIBRARY(LIBAIRSPYHF_LIBRARIES
        NAMES airspyhf
        HINTS $ENV{LIBAIRSPYHF_DIR}/lib ${PC_LIBAIRSPYHF_LIBDIR}
        PATHS /usr/local/lib /usr/lib /opt/local/lib ${PC_LIBAIRSPYHF_LIBDIR} ${PC_LIBAIRSPYHF_LIBRARY_DIRS} ${CMAKE_SOURCE_DIR}/../libairspyhfhf/src
)

find_package_handle_standard_args(LIBAIRSPYHF DEFAULT_MSG LIBAIRSPYHF_LIBRARIES LIBAIRSPYHF_INCLUDE_DIR)
mark_as_advanced(LIBAIRSPYHF_INCLUDE_DIR LIBAIRSPYHF_LIBRARIES)

if (LIBAIRSPYHF_FOUND AND NOT DEFINED libairspyhf::libairspyhf)
    add_library(libairspyhf::libairspyhf INTERFACE IMPORTED)
    set_target_properties(libairspyhf::libairspyhf PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${LIBAIRSPYHF_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${LIBAIRSPYHF_LIBRARIES}"
    )
    target_link_libraries(libairspyhf::libairspyhf INTERFACE libusb::libusb)
endif ()