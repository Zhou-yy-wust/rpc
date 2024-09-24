//
// Created by blair on 2024/9/20.
//


#include "rpcchannel.h"
#include "coder.h"
#include "rpc.pb.h"
#include "rpccontroller.h"
#include "zkclient.h"

#include <arpa/inet.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>



namespace rpc
{
    void RpcChannel::CallMethod(
        const google::protobuf::MethodDescriptor *method,
        google::protobuf::RpcController *controller,
        const google::protobuf::Message *request,
        google::protobuf::Message *response,
        google::protobuf::Closure *done)
    {
        // 构造请求消息
        const std::shared_ptr<protocol::RpcRequest> rpc_request_ptr = Coder::ConstructRpcRequest(method, request);
        const std::shared_ptr<std::string> send_str_ptr = Coder::SerializeToString(*rpc_request_ptr);

        // 向zookeeper请求服务器ip,port
        ZkClient zk_client;
        zk_client.start();
        std::string method_path = "/" + rpc_request_ptr->header().service() + "/" +  rpc_request_ptr->header().method();
        std::string ipport = zk_client.getData(method_path.c_str());
        auto it = ipport.find(':');
        const int port = static_cast<int>(strtol(
            ipport.substr(it+1, ipport.size() - it).c_str(),
            nullptr,
            10
            ));
        const auto ip =ipport.substr(0, it) ;

        // 发送和接收请求
        int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(socket_fd == -1)
        {
            controller->SetFailed("Failed to create socket.");
            return;
        }

        const std::shared_ptr<int> sockte_fd_guard(&socket_fd, [](const int * p){close(*p);});
        SendToRpcServer(controller, sockte_fd_guard, ip, port,*send_str_ptr);
        if(controller->Failed()) return;

        const std::shared_ptr<std::string> response_str_ptr = RecvFromRpcServer(controller, sockte_fd_guard);
        if(controller->Failed()) return;

        // 反序列化，构造响应
        protocol::RpcResponse rpc_response;
        try
        {
            Coder::ParseFromString(rpc_response, *response_str_ptr);
        }catch (const ParseError& e)
        {
            controller->SetFailed(std::string("Rpc response: ") + e.what());
            return;
        }

        Coder::ShowResponseInfo(rpc_response);

        if(const protocol::ResponseBody& body = rpc_response.body(); body.success())
        {
            try
            {
                response->ParseFromString(body.result());
            }catch (const ParseError& e)
            {
                controller->SetFailed(std::string("Result response: ") + e.what());
            }
        }
        else
            controller->SetFailed("Rpc call failed, error: " + body.error_message());

    }


    void RpcChannel::SendToRpcServer(
        google::protobuf::RpcController *controller,
        const std::shared_ptr<int>& sockte_fd_guard,
        const std::string& peer_ip,
        const int peer_port,
        const std::string& message
        )
    {
        sockaddr_in peerAddr{};
        peerAddr.sin_family = AF_INET;
        peerAddr.sin_port = htons(peer_port);
        peerAddr.sin_addr.s_addr = inet_addr(peer_ip.c_str());

        if(connect(
            *sockte_fd_guard,
            reinterpret_cast<sockaddr*>(&peerAddr),
            sizeof peerAddr
            ) == -1)
        {
            controller->SetFailed("Falied to connect to server: " + peer_ip + ":" + std::to_string(peer_port));
            return;
        }

        std::cout << "Connect to server: " << peer_ip << ":" << peer_port << std::endl;

        if(send(*sockte_fd_guard, message.c_str(), message.size(), 0) == -1)
        {
            controller->SetFailed("Falied to send message to server: " + peer_ip + ":" + std::to_string(peer_port));
            return;
        }
        std::cout << "Send message to server: " << peer_ip << ":" << peer_port << std::endl;
    }


    auto RpcChannel::RecvFromRpcServer(
        google::protobuf::RpcController *controller,
        const std::shared_ptr<int>& sockte_fd_guard
        ) -> std::shared_ptr<std::string>
    {
        char recv_buf[1024] = {0};
        size_t bytes_received  = 0;
        if((bytes_received = recv(*sockte_fd_guard, recv_buf, 1024, 0)) == -1)
        {
            controller->SetFailed("Failed to recive from rpc server");
            return {};
        }
        auto response_str_ptr = std::make_shared<std::string>(recv_buf, bytes_received);
        std::cout << "Revived " << response_str_ptr->size() << " bytes from rpc server" << std::endl;
        return response_str_ptr;
    }
}