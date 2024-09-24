//
// Created by blair on 2024/9/17.
//
#include <iostream>
#include <string>
#include "user.pb.h"
#include <rpc.h>
#include <Logging.h>


class UserService: public fixbug::UserServiceRpc{
public:
    bool Login(const std::string& name, const std::string& pwd)
    {
        LOG_INFO << "doing local service: Login";
        LOG_INFO << "name: " << name << " pwd: " << pwd ;
        return true;
    }

    void Login(
        google::protobuf::RpcController *controller,
        const fixbug::LoginRequest *request,
        fixbug::LoginResponse *response,
        google::protobuf::Closure *done) override
    {
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool loginResult = Login(name, pwd);

        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(loginResult);

        done->Run();
    }


};


int main(int argc, char** argv){
    rpc::RpcApp::Init(argc, argv);  // prover

    rpc::RpcProvider provider;
    provider.NotifyService(new UserService());
    provider.PrintServiceMap();
    provider.Run();
    return 0;
}