protobuf_generate_cpp(Protobuf_SRCS Protobuf_HDRS ../user.proto)

add_executable(comsumer calluserservice.cpp ${Protobuf_SRCS})

target_include_directories(comsumer PUBLIC
        ${Protobuf_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
)


target_link_libraries(
        comsumer PUBLIC
        ${Protobuf_LIBRARIES}
        rpc
        rpc_base

)