add_library(cvb fdlist.c)
target_link_libraries(cvb PUBLIC compiler_flags)
target_include_directories(cvb PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
