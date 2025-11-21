add_executable(clnt
    start.c
    clnt.c
    auth.c
    cmd.c)

target_include_directories(clnt
    PRIVATE
    "${PROJECT_SOURCE_DIR}/cvbsh/clnt/include")

target_link_libraries(clnt
    PRIVATE
    cvb)
