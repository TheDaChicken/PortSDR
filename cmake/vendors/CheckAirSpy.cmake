if (NOT AIRSPY_FOUND)
    find_package(AIRSPY)
endif ()

set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)

if (NOT AIRSPY_FOUND)
    add_cmake_external_dependency(
            NAME AIRSPY
            GIT_REPOSITORY https://github.com/airspy/airspyone_host.git
            GIT_TAG c6721000f19601512e9ba6b0340e5d9ced22a900
            WIN_LIB_PATH bin/libairspy.a
            UNIX_LIB_PATH lib/libairspy.a
            TARGETS LibUSB::LibUSB
            EXTRA_CMAKE_ARGS
            -DLIBUSB_LIBRARIES:STRING=${LibUSB_LIBRARIES}
            -DLIBUSB_INCLUDE_DIR:STRING=${LibUSB_INCLUDE_DIR}
            -DTHREADS_PTHREADS_WIN32_LIBRARY=OFF
    )

    list(APPEND PortSDR_DEPENDENCIES AIRSPY::AIRSPY)
    set(AIRSPY_FOUND TRUE)
endif ()

if (NOT AIRSPYHF_FOUND)
    find_package(AIRSPYHF)
endif ()

if (NOT AIRSPYHF_FOUND)
    add_cmake_external_dependency(
            NAME AIRSPYHF
            GIT_REPOSITORY https://github.com/airspy/airspyhf.git
            GIT_TAG 87cf12a30f3a0f10f313aab8e54999ca69b753af
            WIN_LIB_PATH bin/libairspyhf.a
            UNIX_LIB_PATH lib/libairspyhf.a
            TARGETS LibUSB::LibUSB
            EXTRA_CMAKE_ARGS
            -DLIBUSB_LIBRARIES:STRING=${LibUSB_LIBRARIES}
            -DLIBUSB_INCLUDE_DIR:STRING=${LibUSB_INCLUDE_DIR}
            -DTHREADS_PTHREADS_WIN32_LIBRARY=OFF
    )
    list(APPEND PortSDR_DEPENDENCIES AIRSPYHF::AIRSPYHF)
    set(LIBAIRSPYHF_FOUND TRUE)
endif ()