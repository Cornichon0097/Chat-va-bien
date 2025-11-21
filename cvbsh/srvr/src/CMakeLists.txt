add_executable(srvr
    start.c
    srvr.c)

target_include_directories(srvr
    PRIVATE
    "${PROJECT_SOURCE_DIR}/cvbsh/srvr/include")

target_link_libraries(srvr
    PRIVATE
    cvb)

#find_package(mongoc-1.0 REQUIRED)
#target_link_libraries(srvr PRIVATE mongo::mongoc_shared)
