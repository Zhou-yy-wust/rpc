//
// Created by blair on 2024/9/20.
//

#ifndef CODER_H
#define CODER_H

#include <google/protobuf/message.h>
#include <rpc.pb.h>
#include "rpcexceptions.h"

/*
 * 定义RPC服务协议，包括对字节流的编码与解码
 */

namespace rpc
{
    class Coder
    {

    public:
        static void ShowRequestInfo(const protocol::RpcRequest&);
        static void ShowResponseInfo(const protocol::RpcResponse&);
        static auto bytes_to_hex(const std::string& bytes) -> std::string;


        static auto SerializeToString(const google::protobuf::Message&) -> std::shared_ptr<std::string>;


        static void ParseFromString(google::protobuf::Message&, const std::string&);

        static auto ConstructRpcRequest(
            const google::protobuf::MethodDescriptor *method,
            const google::protobuf::Message *user_request  // 这个request是用户自定义服务的请求格式
            ) -> std::shared_ptr<protocol::RpcRequest>;

        static auto ConstructRpcResponse(
            const protocol::MessageHeader& request_header,  //这是客户端发过来的请求头，里面包含了一些信息
            bool success,
            const std::string& result,
            const std::string& error_message
            ) -> std::shared_ptr<protocol::RpcResponse>;

        static auto ConstructRpcResponseFull(
            uint32_t request_id,
            const std::string& service,
            const std::string& method,
            bool success,
            const std::string& result,
            const std::string& error_message
            ) -> std::shared_ptr<protocol::RpcResponse>;

    private:
        static void ConstructHeader(
            protocol::MessageHeader* header,
            uint32_t request_id,
            const std::string& service,
            const std::string& method,
            uint32_t body_size
            );
    };
}




#endif //CODER_H
