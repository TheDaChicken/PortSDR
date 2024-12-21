# in case Git is not available, we default to "unknown"
set(GIT_HASH "unknown")

# find Git and if available set GIT_HASH variable
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
            COMMAND git log -1 --pretty=format:%h
            OUTPUT_VARIABLE GIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
    )

    if (GIT_HASH STREQUAL "")
        message(WARNING "Git failed, using 'unknown' as hash")
        set(GIT_HASH "unknown")
    endif()
endif()

message(STATUS "Git hash: ${GIT_HASH}")

configure_file(src/PortSDRVersion.cpp.in PortSDRVersion.cpp @ONLY)