add_library(cvb logger.c fdlist.c net.c msg.c)
target_include_directories(cvb PUBLIC "${PROJECT_SOURCE_DIR}/libcvb/include")
