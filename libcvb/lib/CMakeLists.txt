add_library(cvb
    SHARED
    logger.c
    net.c
    fdlist.c
    msg.c)

target_include_directories(cvb
    PUBLIC
    "${PROJECT_SOURCE_DIR}/libcvb/include")

target_compile_definitions(cvb
    PRIVATE
    LOGGER_USE_COLOR)
