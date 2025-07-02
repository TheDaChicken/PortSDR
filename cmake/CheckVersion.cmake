# in case Git is not available, we default to "unknown"
set(GIT_HASH "unknown")

# find Git and if available set GIT_HASH variable
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%h
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if (GIT_HASH STREQUAL "")
        message(WARNING "Git failed, using 'unknown' as hash")
        set(GIT_HASH "unknown")
    endif()
else ()
    message(WARNING "Git not found, using 'unknown' as hash")
    set(GIT_HASH "unknown")
endif()

message(STATUS "Git hash: ${GIT_HASH}")

configure_file(src/PortSDRVersion.cpp.in PortSDRVersion.cpp @ONLY)