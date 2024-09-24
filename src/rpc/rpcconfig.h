//
// Created by blair on 2024/9/18.
//

#ifndef RPCCONFIG_H
#define RPCCONFIG_H

#include <unordered_map>
#include <string>

namespace rpc
{
    class RpcConfig
    {
    public:
        void LoadConfigFile(const char* config_file);
        std::string Load(const std::string& key);
    private:
        std::unordered_map<std::string, std::string> config_map_;
    };
}

#endif //RPCCONFIG_H
