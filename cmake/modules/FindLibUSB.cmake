include(FindPackageHandleStandardArgs)

find_package(PkgConfig)

if (PKG_CONFIG_FOUND)
    pkg_check_modules(PC_LibUSB LibUSB-1.0)
endif ()

find_path(LibUSB_INCLUDE_DIR
        NAMES libusb.h
        HINTS ${PC_LibUSB_INCLUDEDIR}
        ATH_SUFFIXES include libusb include/libusb-1.0
)

find_library(LibUSB_LIBRARIES
        NAMES libusb-1.0.so libusb-1.0.a
        HINTS ${PC_LibUSB_LIBDIR} $ENV{LibUSB_ROOT}/include/LibUSB-1.0
        PATH_SUFFIXES lib
        PATHS /usr/local/lib /usr/lib
)

find_package_handle_standard_args(LibUSB DEFAULT_MSG LibUSB_INCLUDE_DIR LibUSB_LIBRARIES)

if (LibUSB_FOUND AND NOT TARGET LibUSB::LibUSB)
    add_library(LibUSB::LibUSB INTERFACE IMPORTED)
    set_target_properties(LibUSB::LibUSB PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${LibUSB_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${LibUSB_LIBRARIES}"
    )
    mark_as_advanced(LibUSB_INCLUDE_DIR LibUSB_LIBRARIES)
endif ()
