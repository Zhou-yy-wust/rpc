aux_source_directory(. SOURCES)
add_library(rpc_base STATIC ${SOURCES}
        Logging.h
        Logging.cpp
        FixedBuffer.h)
target_include_directories(
        rpc_base PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)