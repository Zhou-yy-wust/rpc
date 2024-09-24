//
// Created by blair on 2024/9/18.
//

#ifndef RPCPROVIDER_H
#define RPCPROVIDER_H


#include <google/protobuf/service.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/EventLoop.h>
#include "rpc.pb.h"
#include <zkclient.h>


namespace rpc
{
    class RpcProvider
    {
    public:
        void NotifyService(google::protobuf::Service *);
        void Run();
        void PrintServiceMap();
    private:
        muduo::net::EventLoop loop_;
        ZkClient zkclient_;

        struct ServiceInfo
        {
            google::protobuf::Service* service;
            std::unordered_map<
                std::string,
                const google::protobuf::MethodDescriptor*> method_map;
        };

        std::unordered_map<std::string, ServiceInfo> service_map_;
        void onConection(const muduo::net::TcpConnectionPtr& coon);
        void onMessage(
            const muduo::net::TcpConnectionPtr& coon,
            muduo::net::Buffer* buffer,
            muduo::Timestamp time);
        void SendRpcResponse(
            const muduo::net::TcpConnectionPtr& coon,
            protocol::RpcResponse* message
            );
        static void RpcCallBack(){}  // do nothing

    };
}
#endif //RPCPROVIDER_H
