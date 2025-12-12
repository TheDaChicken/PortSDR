if (NOT rtlsdr_FOUND)
    find_package(rtlsdr)
endif()

if (rtlsdr_FOUND)
    message(STATUS "RTL-SDR library found")
else ()
    message(FATAL_ERROR "RTL-SDR library not found")
endif ()