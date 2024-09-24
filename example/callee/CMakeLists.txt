
protobuf_generate_cpp(Protobuf_SRCS Protobuf_HDRS ../user.proto)

add_executable(provider userservice.cpp ${Protobuf_SRCS})

target_include_directories(provider PUBLIC
        ${Protobuf_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
)


target_link_libraries(
        provider PUBLIC
        ${Protobuf_LIBRARIES}
        rpc
        rpc_base

)