cmake_minimum_required(VERSION 3.20)

project(logger
        VERSION 0.1
        LANGUAGES C)

include(cmake/enable-coloration.cmake)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(lib)
add_subdirectory(examples)
