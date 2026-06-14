if (NOT RTLSDR_FOUND)
    find_package(RTLSDR)
endif()

if (NOT RTLSDR_FOUND)
    add_cmake_external_dependency(
            NAME RTLSDR
            GIT_REPOSITORY https://gitea.osmocom.org/sdr/rtl-sdr.git
            GIT_TAG v2.0.2
            WIN_LIB_PATH lib/librtlsdr_static.a
            UNIX_LIB_PATH lib/librtlsdr_static.a
            TARGETS LibUSB::LibUSB
            EXTRA_CMAKE_ARGS
            -DCMAKE_C_FLAGS:STRING="-std=gnu17"
            -DLIBUSB_LIBRARIES:STRING=${LibUSB_LIBRARIES}
            -DLIBUSB_INCLUDE_DIR:STRING=${LibUSB_INCLUDE_DIR}
    )
    list(APPEND PortSDR_DEPENDENCIES RTLSDR::RTLSDR)
    set(LIBRTLSDR_FOUND TRUE)
endif ()