//
// Created by blair on 2024/9/18.
//

#include "rpcconfig.h"
#include <yaml-cpp/yaml.h>

namespace rpc
{
    std::string RpcConfig::Load(const std::string &key)
    {
        if(const auto it = config_map_.find(key); it != config_map_.end())
        {
            return it->second;
        }
        return "";
    }


    void RpcConfig::LoadConfigFile(const char *config_file)
    {
        YAML::Node config = YAML::LoadFile(config_file);
        for(auto it = config.begin(); it != config.end(); ++it)
        {

            const auto key = it->first.as<std::string>();
            const auto value = it->second.as<std::string>();
            // std::cout << key << ": " << value << std::endl;
            config_map_.insert({key, value});
        }
    }
}
