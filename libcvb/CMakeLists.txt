add_subdirectory(lib)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(unittests)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_subdirectory(docs)
endif()
