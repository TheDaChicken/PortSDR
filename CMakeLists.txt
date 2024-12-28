cmake_minimum_required(VERSION 3.30)
project(PortSDR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(LIBRARY_BUILD_SHARED "Build shared library" OFF)
option(LIBRARY_TESTS "Build tests" ON)
option(SDR_BACKEND_RTLSDR "Enable RTL-SDR backend" ON)
option(SDR_BACKEND_AIRSPY "Enable Airspy backend" OFF)

set(PortSDR_VERSION_MAJOR 1)
set(PortSDR_VERSION_MINOR 0)
set(PortSDR_VERSION_PATCH 0)

set(PortSDR_PUBLIC_HEADER include/PortSDR.h)

include(ExternalProject)
include(FetchContent)

if (LIBRARY_BUILD_SHARED)
    set(LIBRARY_BUILD_TYPE SHARED)
else ()
    set(LIBRARY_BUILD_TYPE STATIC
    )
endif ()

include(cmake/CheckVersion.cmake)

set(PortSDR_LIBRARIES "")
set(PortSDR_VENDOR_FILES "")

if (SDR_BACKEND_RTLSDR)
    include(cmake/vendors/CheckRTL.cmake)

    set(PortSDR_VENDOR_FILES ${PortSDR_VENDOR_FILES}
            src/vendors/RTLSDR.cpp
    )
    set(PortSDR_LIBRARIES ${PortSDR_LIBRARIES} RTLSDR::RTLSDR)
endif ()

if (SDR_BACKEND_AIRSPY)
    include(cmake/vendors/CheckAirSpy.cmake)

    set(PortSDR_VENDOR_FILES ${PortSDR_VENDOR_FILES}
            src/vendors/Airspy.cpp
            include/AirSpy.h
    )
    set(PortSDR_LIBRARIES ${PortSDR_LIBRARIES} LIBAIRSPY::LIBAIRSPY)
endif ()

add_library(PortSDR
        ${LIBRARY_BUILD_TYPE}
        src/PortSDR.cpp
        src/vendors/RTLSDR.cpp
        src/IQConverter.cpp
        ${PortSDR_VENDOR_FILES}
)

target_include_directories(PortSDR PUBLIC include)
target_include_directories(PortSDR PRIVATE src)
target_include_directories(PortSDR PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

set_target_properties(PortSDR PROPERTIES PUBLIC_HEADER ${PortSDR_PUBLIC_HEADER})
set_target_properties(PortSDR PROPERTIES VERSION ${PortSDR_VERSION_MAJOR}.${PortSDR_VERSION_MINOR})
set_target_properties(PortSDR PROPERTIES SOVERSION ${PortSDR_VERSION_MAJOR})
set_target_properties(PortSDR PROPERTIES OUTPUT_NAME "PortSDR")
set_target_properties(PortSDR PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(PortSDR PUBLIC ${PortSDR_LIBRARIES})

if (SDR_BACKEND_RTLSDR)
    target_compile_definitions(PortSDR PUBLIC RTLSDR_SUPPORT=ON)
endif ()

if (SDR_BACKEND_AIRSPY)
    target_compile_definitions(PortSDR PUBLIC AIRSPY_SUPPORT=ON)
endif ()

if (LIBRARY_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

install(TARGETS PortSDR
        PUBLIC_HEADER DESTINATION include
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)