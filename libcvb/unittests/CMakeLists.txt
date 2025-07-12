add_executable(test_fdlist test_fdlist.c)
target_link_libraries(test_fdlist PRIVATE cvb)
add_test(NAME fdlist COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_fdlist)
