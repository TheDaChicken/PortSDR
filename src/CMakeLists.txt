set(PortSDR_LIBRARY_NAME "PortSDR")

if (RTLSDR_FOUND)
    list(APPEND PortSDR_VENDOR_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/../include/RTLSDR.h
            vendors/RTLSDR.cpp
    )
    list(APPEND PortSDR_LIBRARIES RTLSDR::RTLSDR)
endif ()

if (LIBAIRSPY_FOUND)
    list(APPEND PortSDR_VENDOR_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/../include/AirSpy.h
            vendors/AirSpy.cpp
    )
    list(APPEND PortSDR_LIBRARIES LIBAIRSPY::LIBAIRSPY)
endif ()

add_library(${PortSDR_LIBRARY_NAME}
        ${LIBRARY_BUILD_TYPE}
        PortSDR.cpp
        vendors/RTLSDR.cpp
        IQConverter.cpp
        ${PortSDR_VENDOR_FILES}
)

target_include_directories(${PortSDR_LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(${PortSDR_LIBRARY_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PortSDR_LIBRARY_NAME} PRIVATE "${CMAKE_BINARY_DIR}")

set_target_properties(${PortSDR_LIBRARY_NAME} PROPERTIES PUBLIC_HEADER ${PortSDR_PUBLIC_HEADER})
set_target_properties(${PortSDR_LIBRARY_NAME} PROPERTIES VERSION ${PortSDR_VERSION_MAJOR}.${PortSDR_VERSION_MINOR})
set_target_properties(${PortSDR_LIBRARY_NAME} PROPERTIES SOVERSION ${PortSDR_VERSION_MAJOR})
set_target_properties(${PortSDR_LIBRARY_NAME} PROPERTIES OUTPUT_NAME ${PortSDR_LIBRARY_NAME})
set_target_properties(${PortSDR_LIBRARY_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(${PortSDR_LIBRARY_NAME} PUBLIC ${PortSDR_LIBRARIES})

if (RTLSDR_FOUND)
    target_compile_definitions(${PortSDR_LIBRARY_NAME} PUBLIC RTLSDR_SUPPORT=ON)
endif ()

if (LIBAIRSPY_FOUND)
    target_compile_definitions(${PortSDR_LIBRARY_NAME} PUBLIC AIRSPY_SUPPORT=ON)
endif ()

install(TARGETS ${PortSDR_LIBRARY_NAME}
        PUBLIC_HEADER DESTINATION include
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)
