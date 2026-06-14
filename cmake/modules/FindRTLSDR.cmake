include(FindPackageHandleStandardArgs)

find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(PC_RTLSDR RTLSDR QUIET)
endif ()

if (NOT LibUSB_FOUND)
    find_package(LibUSB REQUIRED)
endif()

find_path(RTLSDR_INCLUDE_DIR
        rtl-sdr.h
        PATHS ${PC_RTLSDR_INCLUDEDIR}
        DOC "RTLSDR include directory"
)
find_library(RTLSDR_LIBRARIES
        NAMES libRTLSDR_static libRTLSDR RTLSDR
        PATHS ${PC_RTLSDR_LIBDIR}
        DOC "RTLSDR library location"
)

find_package_handle_standard_args(RTLSDR DEFAULT_MSG
        RTLSDR_INCLUDE_DIR RTLSDR_LIBRARIES
)

if (RTLSDR_FOUND AND NOT TARGET RTLSDR::RTLSDR)
    add_library(RTLSDR::RTLSDR INTERFACE IMPORTED)
    set_target_properties(RTLSDR::RTLSDR PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${RTLSDR_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${RTLSDR_LIBRARIES}"
    )
    target_link_libraries(RTLSDR::RTLSDR INTERFACE LibUSB::LibUSB)

    mark_as_advanced(
            RTLSDR_LIBRARIES
            RTLSDR_STATIC_LIBRARIES
            RTLSDR_INCLUDE_DIR
    )
endif ()
