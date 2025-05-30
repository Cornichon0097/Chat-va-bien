add_executable(srvr start.c db.c)
target_link_libraries(srvr PRIVATE cvb log compiler_flags)
target_include_directories(srvr PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(mongoc-1.0 1.7 REQUIRED)
target_link_libraries(srvr PRIVATE mongo::mongoc_shared)
