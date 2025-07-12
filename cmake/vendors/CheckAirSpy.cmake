if (NOT LIBAIRSPY_FOUND)
    find_package(LIBAIRSPY)
endif ()

if (NOT LIBAIRSPY_FOUND)
    message(STATUS "Building LIBAIRSPY from source.")

    set(LIBAIRSPY_PREFIX "${CMAKE_BINARY_DIR}/libairspy")
    set(LIBAIRSPY_INCLUDE_DIR "${LIBAIRSPY_PREFIX}/include/")
    if (WIN32)
        set(LIBAIRSPY_LIBRARIES "${LIBAIRSPY_PREFIX}/bin/libairspy.a")
    else ()
        set(LIBAIRSPY_LIBRARIES "${LIBAIRSPY_PREFIX}/lib/libairspy.a")
    endif ()

    if (NOT LIBUSB_FOUND)
        find_package(LIBUSB REQUIRED)
    endif ()

    # must be external project because of the uninstall target in the CMakeLists.txt
    ExternalProject_Add(LibAirSpyExternal
            GIT_REPOSITORY https://github.com/airspy/airspyone_host.git
            GIT_TAG 0bccf3b88830d3e5ec029cfb42b9d4c2f2ec8554
            PREFIX ${LIBAIRSPY_PREFIX}
            UPDATE_COMMAND ""
            BUILD_BYPRODUCTS ${LIBAIRSPY_LIBRARIES}
            CMAKE_ARGS
            ${COMMON_CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX:STRING=${LIBAIRSPY_PREFIX}
            -DLIBUSB_LIBRARIES:STRING=${LIBUSB_LIBRARIES}
            -DLIBUSB_INCLUDE_DIR:STRING=${LIBUSB_INCLUDE_DIR}
    )
    file(MAKE_DIRECTORY ${LIBAIRSPY_INCLUDE_DIR})

    add_library(libairspy::libairspy INTERFACE IMPORTED GLOBAL)
    set_target_properties(libairspy::libairspy PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${LIBAIRSPY_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${LIBAIRSPY_LIBRARIES}"
    )
    target_link_libraries(libairspy::libairspy INTERFACE libusb::libusb)

    add_dependencies(libairspy::libairspy LibAirSpyExternal)

    list(APPEND PortSDR_DEPENDENCIES libairspy::libairspy)
    set(LIBAIRSPY_FOUND TRUE)
endif ()

if (NOT LIBAIRSPYHF_FOUND)
    find_package(LIBAIRSPYHF)
endif ()

if (NOT LIBAIRSPYHF_FOUND)
    message(STATUS "Building LIBAIRSPYHF from source.")

    set(LIBAIRSPYHF_PREFIX "${CMAKE_BINARY_DIR}/libairspyhf")
    set(LIBAIRSPYHF_INCLUDE_DIR "${LIBAIRSPYHF_PREFIX}/include/")
    if (WIN32)
        set(LIBAIRSPYHF_LIBRARIES "${LIBAIRSPYHF_PREFIX}/bin/libairspyhf.a")
    else ()
        set(LIBAIRSPYHF_LIBRARIES "${LIBAIRSPYHF_PREFIX}/lib/libairspyhf.a")
    endif ()

    if (NOT LIBUSB_FOUND)
        find_package(LIBUSB REQUIRED)
    endif ()

    # must be external project because of the uninstall target in the CMakeLists.txt
    ExternalProject_Add(LibAirSpyHFExternal
            GIT_REPOSITORY https://github.com/thedachicken/airspyhf.git
            GIT_TAG use-after-free
            PREFIX ${LIBAIRSPYHF_PREFIX}
            UPDATE_COMMAND ""
            BUILD_BYPRODUCTS ${LIBAIRSPYHF_LIBRARIES}
            CMAKE_ARGS
            ${COMMON_CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX:STRING=${LIBAIRSPYHF_PREFIX}
            -DLIBUSB_LIBRARIES:STRING=${LIBUSB_LIBRARIES}
            -DLIBUSB_INCLUDE_DIR:STRING=${LIBUSB_INCLUDE_DIR}
            -DTHREADS_PTHREADS_WIN32_LIBRARY=OFF
    )
    file(MAKE_DIRECTORY ${LIBAIRSPYHF_INCLUDE_DIR})

    add_library(libairspyhf::libairspyhf INTERFACE IMPORTED GLOBAL)
    set_target_properties(libairspyhf::libairspyhf PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${LIBAIRSPYHF_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${LIBAIRSPYHF_LIBRARIES}"
    )
    target_link_libraries(libairspyhf::libairspyhf INTERFACE libusb::libusb)

    add_dependencies(libairspyhf::libairspyhf LibAirSpyHFExternal)

    list(APPEND PortSDR_DEPENDENCIES LibAirSpyHFExternal)
    set(LIBAIRSPYHF_FOUND TRUE)
endif ()