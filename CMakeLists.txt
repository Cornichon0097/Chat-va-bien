cmake_minimum_required(VERSION 3.20)

project(chat-va-bien
        VERSION 0.1
        DESCRIPTION "Group chat app based on home-made RFC"
        HOMEPAGE_URL "https://github.com/Cornichon0097/Chat-va-bien"
        LANGUAGES C)

include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

add_library(c_flags
    INTERFACE)
target_compile_options(c_flags
    INTERFACE
    -Wall
    -Wextra
    -Wwrite-strings
    -Wstrict-prototypes
    -Wunreachable-code
    -Werror)

add_subdirectory(libcvb)
add_subdirectory(cvbsh)
