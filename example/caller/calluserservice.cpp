//
// Created by blair on 2024/9/20.
//


#include <iostream>
#include <rpc.h>
#include <user.pb.h>


int main(int argc, char* argv[])
{
    rpc::RpcApp::Init(argc, argv);
    fixbug::UserServiceRpc_Stub stub(new rpc::RpcChannel());
    fixbug::LoginRequest request;
    request.set_name("zhangsan");
    request.set_pwd("123456");

    fixbug::LoginResponse response;
    rpc::RpcController controller;
    stub.Login(&controller, &request, &response, nullptr);

    if(controller.Failed())
    {
        std::cout << "Failed to promote procudure call, error message: " << controller.ErrorText() << std::endl;
    }
    else
    {
        if(response.result().errcode() == 0)
            std::cout << "rpc login response: " << response.success() << std::endl;
        else
            std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
    }
    return 0;
}
