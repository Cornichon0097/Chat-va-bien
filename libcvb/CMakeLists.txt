configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cvb/version.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cvb/version.h")

add_library(cvb fdlist.c net.c)
target_link_libraries(cvb PRIVATE log compiler_flags)
target_include_directories(cvb PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
