set(PortSDR_LIBRARY_NAME "PortSDR")

set(PortSDR_VENDOR_FILES "")
set(PortSDR_COMPILE_DEFINITIONS "")

set(PortSDR_PUBLIC_HEADER
    ../include/PortSDR.h
    ../include/Ranges.h
    ../include/Device.h
    ../include/Stream.h
    ../include/Error.h
    ../include/HostType.h
)

if (RTLSDR_FOUND)
    list(APPEND PortSDR_VENDOR_FILES
            vendors/RTLSDR.h
            vendors/RTLSDR.cpp
    )
    list(APPEND PortSDR_COMPILE_DEFINITIONS RTLSDR_SUPPORT=ON)
    list(APPEND PortSDR_LIBRARIES RTLSDR::RTLSDR)
endif ()

if (AIRSPY_FOUND)
    list(APPEND PortSDR_VENDOR_FILES
            vendors/AirSpy.h
            vendors/AirSpy.cpp
    )
    list(APPEND PortSDR_COMPILE_DEFINITIONS AIRSPY_SUPPORT=ON)
    list(APPEND PortSDR_LIBRARIES AIRSPY::AIRSPY)
endif ()

if (AIRSPYHF_FOUND)
    list(APPEND PortSDR_VENDOR_FILES
            vendors/AirSpyHf.h
            vendors/AirSpyHf.cpp
    )
    list(APPEND PortSDR_COMPILE_DEFINITIONS AIRSPYHF_SUPPORT=ON)
    list(APPEND PortSDR_LIBRARIES AIRSPYHF::AIRSPYHF)
endif()

add_library(${PortSDR_LIBRARY_NAME}
        ${LIBRARY_BUILD_TYPE}
        PortSDR.cpp
        Utils.h
        Host.h
        ${PortSDR_VENDOR_FILES}
        ${PortSDR_PUBLIC_HEADER}
)

target_include_directories(${PortSDR_LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(${PortSDR_LIBRARY_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../)

set_target_properties(${PortSDR_LIBRARY_NAME} PROPERTIES PUBLIC_HEADER "${PortSDR_PUBLIC_HEADER}")
set_target_properties(${PortSDR_LIBRARY_NAME} PROPERTIES VERSION ${PortSDR_VERSION_MAJOR}.${PortSDR_VERSION_MINOR})
set_target_properties(${PortSDR_LIBRARY_NAME} PROPERTIES SOVERSION ${PortSDR_VERSION_MAJOR})
set_target_properties(${PortSDR_LIBRARY_NAME} PROPERTIES OUTPUT_NAME ${PortSDR_LIBRARY_NAME})
set_target_properties(${PortSDR_LIBRARY_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(${PortSDR_LIBRARY_NAME} PUBLIC ${PortSDR_LIBRARIES})
target_compile_definitions(${PortSDR_LIBRARY_NAME} PUBLIC ${PortSDR_COMPILE_DEFINITIONS})

install(TARGETS ${PortSDR_LIBRARY_NAME}
        PUBLIC_HEADER DESTINATION include
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)
