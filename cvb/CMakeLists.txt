configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/version.h")

add_executable(srvr srvr.c net.c)
target_link_libraries(srvr PUBLIC log cvb compiler_flags)
target_include_directories(srvr PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(clnt clnt.c net.c)
target_link_libraries(clnt PUBLIC log cvb compiler_flags)
target_include_directories(clnt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
