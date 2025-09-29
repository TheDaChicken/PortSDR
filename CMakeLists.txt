cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(LIBRARY_BUILD_SHARED "Build shared library" OFF)
option(LIBRARY_TESTS "Build tests" ON)
option(SDR_BACKEND_RTLSDR "Enable RTL-SDR backend" ON)
option(SDR_BACKEND_AIRSPY "Enable Airspy backend" OFF)

include(ExternalProject)
include(FetchContent)

set(PortSDR_VERSION_MAJOR 1)
set(PortSDR_VERSION_MINOR 0)
set(PortSDR_VERSION_PATCH 0)
set(PortSDR_PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/PortSDR.h)

project(PortSDR VERSION ${PortSDR_VERSION_MAJOR}.${PortSDR_VERSION_MINOR}.${PortSDR_VERSION_PATCH})

if (LIBRARY_BUILD_SHARED)
    set(LIBRARY_BUILD_TYPE SHARED)
else ()
    set(LIBRARY_BUILD_TYPE STATIC)
endif ()

include(cmake/CheckVersion.cmake)

set(PortSDR_LIBRARIES "")
set(PortSDR_VENDOR_FILES "")

if (SDR_BACKEND_RTLSDR)
    include(cmake/vendors/CheckRTL.cmake)
endif ()

if (SDR_BACKEND_AIRSPY)
    include(cmake/vendors/CheckAirSpy.cmake)
endif ()

add_subdirectory(src)

if (LIBRARY_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()
