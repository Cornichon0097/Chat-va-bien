add_library(cvb
    SHARED
    fdlist.c
    logger.c
    msg.c
    net.c)

target_include_directories(cvb
    PUBLIC
    "${PROJECT_SOURCE_DIR}/libcvb/include")

target_compile_definitions(cvb
    PRIVATE
    LOGGER_USE_COLOR)
