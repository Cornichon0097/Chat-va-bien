add_subdirectory(lib)
add_subdirectory(unittests)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_subdirectory(docs)
endif()
