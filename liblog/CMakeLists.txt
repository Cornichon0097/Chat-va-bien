add_library(log logger.c)
target_link_libraries(log PUBLIC compiler_flags)
target_include_directories(log PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
add_compile_definitions(LOG_USE_COLOR)
