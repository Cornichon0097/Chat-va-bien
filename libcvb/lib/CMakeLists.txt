add_library(cvb fdlist.c net.c msg.c)
target_link_libraries(cvb PRIVATE logger)
target_include_directories(cvb PUBLIC "${PROJECT_SOURCE_DIR}/libcvb/include")
