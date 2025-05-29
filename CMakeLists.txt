cmake_minimum_required(VERSION 3.25)

project(CVB
        VERSION 0.1
        DESCRIPTION "Group chat app based on home-made RFC"
        HOMEPAGE_URL "https://github.com/Cornichon0097/Chat-va-bien"
        LANGUAGES C)

option(BUILD_DOC "Build documentation" OFF)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE c_std_90)

target_compile_options(compiler_flags INTERFACE
    $<$<C_COMPILER_ID:GNU>:$<BUILD_INTERFACE:
        -Wall;
        -Wextra;
        -Wwrite-strings;
        -Wstrict-prototypes;
        -Wunreachable-code;
        -Werror>>

    $<$<C_COMPILER_ID:Clang>:$<BUILD_INTERFACE:
        -Wall;
        -Werror>>

    $<$<C_COMPILER_ID:MSVC>:$<BUILD_INTERFACE:
        /Wall>>)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/liblog)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libcvb)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/srvr)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/clnt)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)

if (BUILD_DOC)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/docs)
endif (BUILD_DOC)
