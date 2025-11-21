add_executable(test_fdlist
    test_fdlist.c)

target_link_libraries(test_fdlist
    PRIVATE
    cvb)

add_test(NAME TestFDList
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_fdlist)
