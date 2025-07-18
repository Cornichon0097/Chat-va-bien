cmake_minimum_required(VERSION 3.20)

project(chat-va-bien
        VERSION 0.1
        DESCRIPTION "Group chat app based on home-made RFC"
        HOMEPAGE_URL "https://github.com/Cornichon0097/Chat-va-bien"
        LANGUAGES C)

include(CTest)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

add_subdirectory(libcvb)
# add_subdirectory(cvbsh)
