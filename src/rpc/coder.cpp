//
// Created by blair on 2024/9/20.
//

#include "coder.h"


#include <iostream>
#include <iomanip> // 用于十六进制输出

namespace rpc
{
    // 辅助函数：将 bytes 转换为十六进制字符串
    auto Coder::bytes_to_hex(const std::string& bytes) -> std::string {
        std::ostringstream oss;
        for (unsigned char c : bytes) {
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
        }
        return oss.str();
    }

    void Coder::ShowRequestInfo(const protocol::RpcRequest& request) {
        const protocol::MessageHeader& header = request.header();

        // 打印消息头相关信息
        std::cout << "Message Header: " << std::endl;
        std::cout << "  Header Size: " << header.header_size() << std::endl;
        std::cout << "  Request ID: " << header.request_id() << std::endl;
        std::cout << "  Service Name: " << header.service() << std::endl;  // bytes 可能包含二进制数据
        std::cout << "  Method Name: " << header.method() << std::endl;   // bytes 可能包含二进制数据
        std::cout << "  Body Size: " << header.body_size() << std::endl;

        const protocol::RequestBody& body = request.body();

        // 打印消息体相关信息
        std::cout << "Request Body: " << std::endl;
        std::cout << "  Params: " << Coder::bytes_to_hex(body.params()) << std::endl;  // bytes 可能包含二进制数据
    }


    void Coder::ShowResponseInfo(const protocol::RpcResponse& response) {
        // 打印消息头
        const protocol::MessageHeader& header = response.header();
        std::cout << "Message Header:" << std::endl;
        std::cout << "  Header Size: " << header.header_size() << std::endl;
        std::cout << "  Request ID: " << header.request_id() << std::endl;
        std::cout << "  Service: " << header.service() << std::endl;
        std::cout << "  Method: " << header.method() << std::endl;
        std::cout << "  Body Size: " << header.body_size() << std::endl;

        // 打印响应消息体
        const protocol::ResponseBody& body = response.body();
        std::cout << "Response Body:" << std::endl;
        std::cout << "  Success: " << (body.success() ? "true" : "false") << std::endl;

        if (body.success()) {
            std::cout << "  Result: " << Coder::bytes_to_hex(body.result()) << std::endl;
        } else {
            std::cout << "  Error Message: " << body.error_message() << std::endl;
        }
    }


    auto Coder::SerializeToString(const google::protobuf::Message& message) -> std::shared_ptr<std::string>
    {

        auto bytes_ptr = std::make_shared<std::string>();
        if(!message.SerializeToString(bytes_ptr.get()))
            throw SerializeError("Failed to serialize message to the string.", __FILE__, __LINE__);
        return bytes_ptr;
    }


    void Coder::ParseFromString(google::protobuf::Message& message,const std::string& bytes)
    {
        if(!message.ParseFromString(bytes))
            throw ParseError("Failed to parse the message from string.", __FILE__, __LINE__);
    }


    void Coder::ConstructHeader(
        protocol::MessageHeader* header,
        uint32_t request_id,
        const std::string& service,
        const std::string& method,
        uint32_t body_size
        )
    {
        header->set_header_size(12 + service.size() + method.size());
        header->set_request_id(request_id);
        header->set_service(service);
        header->set_method(method);
        header->set_body_size(body_size);
    }

    auto Coder::ConstructRpcRequest (
     const google::protobuf::MethodDescriptor *method,
     const google::protobuf::Message *user_request  // 这个request是用户自定义服务的请求格式
     ) -> std::shared_ptr<protocol::RpcRequest>
    {
        auto request = std::make_shared<protocol::RpcRequest>();
        const auto request_header = request->mutable_header();
        const auto request_body = request->mutable_body();

        // 构造body
        const auto body_bytes_ptr = Coder::SerializeToString(*user_request);
        request_body->set_params(*body_bytes_ptr);
        const uint32_t body_size = body_bytes_ptr->size();

        // 获取服务名字，方法名字
        const auto* service_descriptor = method->service();
        const std::string& method_name = method->name();
        const std::string& service_name = service_descriptor->name();

        // 构造header
        ConstructHeader(
            request_header,
            0,
            service_name,
            method_name,
            body_size);
        return request;
    }


    auto Coder::ConstructRpcResponseFull (
        uint32_t request_id,
        const std::string& service,
        const std::string& method,
        bool success,
        const std::string& result,
        const std::string& error_message
        ) -> std::shared_ptr<protocol::RpcResponse>
    {
        const auto response = std::make_shared<protocol::RpcResponse>();
        const auto response_header = response->mutable_header();
        const auto response_body = response->mutable_body();

        ConstructHeader(
        response_header,
        request_id,
        service,
        method,
        sizeof(bool) + result.size() + error_message.size());

        response_body->set_success(success);
        response_body->set_result(result);
        response_body->set_error_message(error_message);

        return response;
    }


    auto Coder::ConstructRpcResponse(
        const protocol::MessageHeader& request_header,
        bool success,
        const std::string& result,
        const std::string& error_message
        ) -> std::shared_ptr<protocol::RpcResponse>
    {
        return ConstructRpcResponseFull(
            request_header.request_id(),
            request_header.service(),
            request_header.method(),
            success,
            result,
            error_message
        );
    }
}