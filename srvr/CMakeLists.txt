add_executable(srvr start.c)
target_link_libraries(srvr PUBLIC cvb log compiler_flags)
#target_include_directories(start PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
