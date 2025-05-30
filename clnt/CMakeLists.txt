add_executable(clnt start.c)
target_link_libraries(clnt PRIVATE cvb log compiler_flags)
