configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cvb/version.h.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/cvb/version.h")

add_executable(srvr srvr.c fdlist.c)
target_link_libraries(srvr PUBLIC log compiler_flags)
target_include_directories(srvr PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# add_executable(clnt clnt.c)
# target_link_libraries(clnt PUBLIC compiler_flags)
