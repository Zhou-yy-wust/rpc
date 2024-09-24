//
// Created by blair on 2024/9/18.
//

#include "rpcapp.h"
#include <iostream>
#include <unistd.h>

namespace rpc
{
    RpcConfig RpcApp::cfg_;

    void ShowArgsHelp()
    {
        std::cout << "command -i <config_file>" << std::endl;
    }


    void RpcApp::Init(int argc, char** argv)
    {
        if(argc < 2)
        {
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        }

        int c = 0;
        std::string config_file;
        while((c = getopt(argc, argv, "i:")) != -1)
        {
            switch (c)
            {
                case 'i':
                    config_file = optarg;
                break;
                case '?':
                    case ':':
                    ShowArgsHelp();
                exit(EXIT_FAILURE);
                default:
                    break;
            }
        }

        // 开始加载配置文件 rpcserver_ip, rpcserver_port, zookeepker_ip, zookeepker_port
        cfg_.LoadConfigFile(config_file.c_str());
    }

    RpcApp& RpcApp::GetInstance()
    {
        static RpcApp intrance;
        return intrance;
    }


    RpcConfig& RpcApp::GetConfig()
    {
        return cfg_;
    }
}