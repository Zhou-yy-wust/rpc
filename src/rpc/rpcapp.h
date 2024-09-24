//
// Created by blair on 2024/9/18.
//

#ifndef RPCAPP_H
#define RPCAPP_H

#include "rpcconfig.h"

namespace rpc
{
    class RpcApp
    {
    public:
        static void Init(int argc, char** argv);
        static RpcApp& GetInstance();
        static RpcConfig& GetConfig();
        RpcApp(const RpcApp&) = delete;
        RpcApp(RpcApp &&) = delete;
        RpcApp& operator= (const RpcApp&) = delete;
        RpcApp& operator= (RpcApp&&) = delete;
    private:
        RpcApp() = default;
        static RpcConfig cfg_;

    };
}

#endif //RPCAPP_H
