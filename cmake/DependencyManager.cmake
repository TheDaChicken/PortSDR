include(ExternalProject)

if (NOT DEFINED SDR_DEPENDENCIES)
    set(SDR_DEPENDENCIES "" CACHE INTERNAL "All SDR external dependencies")
endif()

function(add_cmake_external_dependency)
    set(options)
    set(oneValueArgs
            NAME
            GIT_REPOSITORY
            GIT_TAG
            WIN_LIB_PATH
            UNIX_LIB_PATH
    )
    set(multiValueArgs
            TARGETS
            EXTRA_CMAKE_ARGS
    )

    cmake_parse_arguments(DEP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT DEP_NAME)
        message(FATAL_ERROR "add_external_dependency: NAME is required")
    endif ()

    set(${DEP_NAME}_PREFIX "${CMAKE_BINARY_DIR}/${DEP_NAME}")
    set(${DEP_NAME}_INCLUDE_DIR "${${DEP_NAME}_PREFIX}/include/")

    if (WIN32)
        set(${DEP_NAME}_LIB_PATH "${${DEP_NAME}_PREFIX}/${DEP_WIN_LIB_PATH}")
    else ()
        set(${DEP_NAME}_LIB_PATH "${${DEP_NAME}_PREFIX}/${DEP_UNIX_LIB_PATH}")
    endif ()

    message(STATUS "Building ${DEP_NAME} from source.")

    ExternalProject_Add(${DEP_NAME}_external
            PREFIX ${${DEP_NAME}_PREFIX}
            GIT_REPOSITORY ${DEP_GIT_REPOSITORY}
            GIT_TAG ${DEP_GIT_TAG}
            UPDATE_COMMAND ""
            BUILD_BYPRODUCTS ${${DEP_NAME}_LIB_PATH}
            CMAKE_ARGS
            ${COMMON_CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX:STRING=${${DEP_NAME}_PREFIX}
            ${DEP_EXTRA_CMAKE_ARGS}
    )

    file(MAKE_DIRECTORY ${${DEP_NAME}_INCLUDE_DIR})

    add_library(${DEP_NAME}::${DEP_NAME} INTERFACE IMPORTED GLOBAL)
    set_target_properties(${DEP_NAME}::${DEP_NAME} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${${DEP_NAME}_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${${DEP_NAME}_LIB_PATH}"
    )
    if (DEP_TARGETS)
        target_link_libraries(${DEP_NAME}::${DEP_NAME}
                INTERFACE ${DEP_TARGETS}
        )
    endif ()

    add_dependencies(${DEP_NAME}::${DEP_NAME} ${DEP_NAME}_external)
endfunction()