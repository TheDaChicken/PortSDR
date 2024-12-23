project(testing)

FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_executable(Tests
        vendors/RTLSDR.cpp
        vendors/AirSpy.cpp
        AnyTests.cpp
)

target_link_libraries(Tests PRIVATE
        PortSDR
        gtest
        gtest_main
        gmock
)

gtest_discover_tests(Tests)