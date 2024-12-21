find_package(RTLSDR)

if (RTLSDR_FOUND)
    message(STATUS "RTL-SDR library found")
else ()
    message(FATAL_ERROR "RTL-SDR library not found")
endif ()