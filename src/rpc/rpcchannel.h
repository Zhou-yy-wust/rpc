//
// Created by blair on 2024/9/20.
//

#ifndef RPCCHANNEL_H
#define RPCCHANNEL_H

#include <google/protobuf/service.h>

namespace rpc
{
    class RpcChannel final :public google::protobuf::RpcChannel
    {
    public:
        void CallMethod(
            const google::protobuf::MethodDescriptor *method,
            google::protobuf::RpcController *controller,
            const google::protobuf::Message *request,
            google::protobuf::Message *response,
            google::protobuf::Closure *done) override;
    private:
        static void SendToRpcServer(
            google::protobuf::RpcController *controller,
            const std::shared_ptr<int>& sockte_fd_guard,
            const std::string& peer_ip,
            const int peer_port,
            const std::string& message
            );
        static auto RecvFromRpcServer(
            google::protobuf::RpcController *controller,
            const std::shared_ptr<int>& sockte_fd_guard
            ) -> std::shared_ptr<std::string>;
    };
}

#endif //RPCCHANNEL_H
