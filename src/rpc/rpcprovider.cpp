//
// Created by blair on 2024/9/18.
//


#include <functional>
#include <muduo/net/TcpServer.h>
#include <google/protobuf/descriptor.h>
#include <muduo/base/ProcessInfo.h>

#include "rpcapp.h"
#include "coder.h"
#include "rpcexceptions.h"
#include "rpcprovider.h"
#include "Logging.h"

namespace rpc
{
    void RpcProvider::PrintServiceMap()
    {
        for(const auto &[service_name, service_info]: service_map_)
        {
            // 打印服务名称
            LOG_INFO << "Show info: " ;
            LOG_INFO << "Service: " << service_name ;
            for(const auto &[method_name, method_descriptor]: service_info.method_map)
            {
                LOG_INFO << "  Method: " << method_name
                          << " (Full method name: " << method_descriptor->full_name() << ")"
                          ;
            }
        }
    }
    
    void RpcProvider::NotifyService(google::protobuf::Service *service)
    {
        ServiceInfo info;
        info.service = service;

        const google::protobuf::ServiceDescriptor *service_descriptor = service->GetDescriptor();
        const auto method_count = service_descriptor->method_count();
        for(int i = 0; i < method_count; ++i)
        {
            const auto *method_desriptor = service_descriptor->method(i);
            info.method_map.insert({method_desriptor->name(), method_desriptor});
        }
        service_map_.insert({service_descriptor->name(), info});
    }


    void RpcProvider::Run()
    {
        auto rpc_app_cfg = RpcApp::GetInstance().GetConfig();
        const std::string ip =rpc_app_cfg.Load("rpcserverip");
        const auto port = strtol(
            rpc_app_cfg.Load("rpcserverport").c_str(), nullptr, 10);
        const muduo::net::InetAddress addr(ip, static_cast<uint16_t>(port));

        muduo::net::TcpServer server(&loop_, addr, "RpcProvider");
        server.setThreadNum(8);  // 1个IO线程，7个工作线程
        server.setConnectionCallback(  // 注册连接 回调
        [this](const muduo::net::TcpConnectionPtr& conn){onConection(conn);}
        );

        server.setMessageCallback(
        [this](
            const muduo::net::TcpConnectionPtr& coon,
            muduo::net::Buffer* buffer,
            muduo::Timestamp time){onMessage(coon, buffer, time);}
            );

        zkclient_.start();
        // 注册服务
        LOG_INFO << "注册服务";
        for(const auto& [service_name, service_info]: service_map_)
        {
            auto service_path = "/" + service_name;
            zkclient_.create(service_path.c_str(), nullptr, 0);
            for(const auto& [method_name, k]: service_info.method_map)
            {
                auto method_path = service_path + "/" + method_name;
                std::string ipport = ip + ":" + std::to_string(port);
                zkclient_.create(method_path.c_str(), ipport.c_str(),
                    static_cast<int>(ipport.size()), ZOO_EPHEMERAL);  // 服务永久性节点，方法临时节点
            }
        }

        server.start();
        loop_.loop();
    }


    void RpcProvider::onConection(const muduo::net::TcpConnectionPtr& conn)
    {
        // 短连接
        LOG_INFO << "RpcProvider - " << conn->peerAddress().toIpPort()
        << " -> " << conn->localAddress().toIpPort()
        << " is " << (conn->connected()? "UP" : "DOWN");
    }


    void RpcProvider::onMessage(
        const muduo::net::TcpConnectionPtr& coon,
        muduo::net::Buffer* buffer,
        muduo::Timestamp time)
    {
        // 解析请求头
        const std::string recv_buf = buffer->retrieveAllAsString();
        protocol::RpcRequest request;
        try
        {
            Coder::ParseFromString(request, recv_buf);
        }catch (const ParseError& e)
        {
            auto error_message = "Invalid message format, time: " + time.toFormattedString();
            const auto rpc_response = Coder::ConstructRpcResponseFull(
                -1, "", "",false, "", error_message);
            SendRpcResponse(coon, rpc_response.get());
            LOG_INFO << e.what() ;
            return;
        }

        // Coder::ShowRequestInfo(request);  // 打印信息
        protocol::MessageHeader header = request.header();
        const auto& service_name = header.service();
        const auto& method_name = header.method();
        const auto& params_bytes = request.body().params();
        ServiceInfo service_info;
        const google::protobuf::MethodDescriptor* methd_disriptor;

        try
        {
            // 获取服务和方法
            const auto it = service_map_.find(service_name);
            if(it == service_map_.end())  // 没有此服务
                throw FindServiceError(service_name + " not find", __FILE__, __LINE__);
            service_info = it->second;

            auto method_map = service_info.method_map;
            auto method_iter = method_map.find(method_name);
            if(method_iter == method_map.end())  // 没有此方法
                throw FindMethodError(method_name + " not find" + " in " + service_name, __FILE__, __LINE__);
            methd_disriptor = method_iter->second;
        }
        catch (const FindServiceError& e)
        {
            auto error_message ="Can not found service: " + service_name + ", time: " + time.toFormattedString();
            const auto rpc_response = Coder::ConstructRpcResponse(
                    header, false, "", error_message);

            SendRpcResponse(coon, rpc_response.get());
            LOG_INFO << e.what() ;
        }
        catch (const FindMethodError& e)
        {
            auto error_message =
                "Can not found method " + method_name + " in " + service_name
            + ", time: " + time.toFormattedString();

            const auto rpc_response = Coder::ConstructRpcResponse(
                    header, false, "", error_message);
            SendRpcResponse(coon, rpc_response.get());
            LOG_INFO << e.what() ;
            return;
        }

        // 执行调用
        google::protobuf::Service* service = service_info.service;
        google::protobuf::Message* user_request = service->GetRequestPrototype(methd_disriptor).New();
        google::protobuf::Message* user_response = service->GetResponsePrototype(methd_disriptor).New();

        try
        {
            Coder::ParseFromString(*user_request, params_bytes);
        }
        catch ( const ParseError& e)
        {
            auto error_message = "Invalid argument format, time: " + time.toFormattedString();
            const auto rpc_response = Coder::ConstructRpcResponse(
                header, false, "", error_message);
            SendRpcResponse(coon, rpc_response.get());
            LOG_INFO << e.what() ;
            return;
        }

        auto* done = google::protobuf::NewCallback(&RpcProvider::RpcCallBack);
        service->CallMethod(methd_disriptor, nullptr, user_request, user_response, done);
        auto result = *Coder::SerializeToString(*user_response);
        const auto rpc_response = Coder::ConstructRpcResponse(header, true, result, "");
        SendRpcResponse(coon, rpc_response.get());
    }


    void RpcProvider::SendRpcResponse(
        const muduo::net::TcpConnectionPtr& coon,
        protocol::RpcResponse* message
        )
    {
        // Coder::ShowResponseInfo(*message);
        std::string response_str;
        if(message->SerializeToString(&response_str))
            coon->send(response_str);
        else
            LOG_INFO << "Serialize response_str error" ;
        LOG_INFO << "Send " << response_str.size() << " bytes to client" ;
        coon->shutdown();
    }
}