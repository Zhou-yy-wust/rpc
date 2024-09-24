//
// Created by blair on 2024/9/23.
//

#include "zkclient.h"
#include "rpcapp.h"
#include "Logging.h"


namespace rpc
{

    void ZkClient::start()
    {
        RpcConfig cfg = RpcApp::GetInstance().GetConfig();
        const auto zk_ip = cfg.Load("zookeeperip");
        const auto zk_port = cfg.Load("zookeeperport");
        const auto zk_host = zk_ip + ":" + zk_port;

        /* zookeeper的API客户端程序提供了三个线程
         * 1. API调用线程
         * 2. 网路IO线程, pthread_create, poll
         * 3. watcher回调线程
         */
        zhandle_ = zookeeper_init(
            zk_host.c_str(),
            &watcher,
            3000,
            nullptr,
            nullptr,
            0);
        if(zhandle_ == nullptr)
        {
            LOG_ERROR << "zookeeper_init error!";
            exit(EXIT_FAILURE);
        }
        zoo_set_context(zhandle_,this);
        connected_.acquire();
        LOG_INFO << "zookeeper_init success!";
    }

    void f(int rc, const struct Stat *stat, const void *data)
    {
        LOG_INFO << "f doing...";
        LOG_INFO << "f done...";

    };

    void ZkClient::create(const char *path, const char *data, int len, int state)
    {
        int flag = zoo_aexists(zhandle_, path, 0, &f, nullptr);
        if(flag == ZOK)
        {
            flag = zoo_acreate(
                zhandle_,
                path,
                data, len,
                &ZOO_OPEN_ACL_UNSAFE, state,
                &stringCompletion, this);
            if(flag == ZOK)
            {
                created_.acquire();
                LOG_INFO << "znode create success, path: " << path << ", flag: " << flag;
            }
            else
            {
                LOG_INFO << "znode create failed, path: " << path << ", flag: " << flag;
                exit(EXIT_FAILURE);
            }

        }
    }


    std::string ZkClient::getData(const char *path)
    {

        int flag = zoo_aget(zhandle_, path, 0,
            &dataCompletion, this);
        if(flag == ZOK)
        {
            data_ready_.acquire();
            LOG_INFO << "znode get success, path: " << path << ", flag: " << flag;
            return result_;
        }

        LOG_INFO << "znode get failed, path: " << path << ", flag: " << flag;
        exit(EXIT_FAILURE);
    }


    void ZkClient::watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx)
    {
        if(type == ZOO_SESSION_EVENT && state == ZOO_CONNECTED_STATE)
        {
            LOG_INFO << "watcher doing...";
            const auto client = static_cast<const ZkClient*>(zoo_get_context(zh));
            client->connected_.release();
            LOG_INFO << "watcher done...";
        }
    }


    void ZkClient::dataCompletion(int rc, const char *value, int value_len, const Stat *stat, const void *data)
    {
        /*
         * rc: ZOK operation completed successfully ,
         * ZNONODE the node does not exist.
         * ZNOAUTH the client does not have permission.
         */
         // 如果获取成功

        LOG_INFO << "dataCompletion doing...";
        const auto client = static_cast<const ZkClient*>(data);
        if(rc == ZOK) client->result_.assign(value, value_len); // 将节点数据存储到 result_ 中
        client->data_ready_.release();
        LOG_INFO << "dataCompletion done...";
    }

    void ZkClient::stringCompletion(int rc, const char *value, const void *data)
    {
        LOG_INFO << "stringCompletion doing...";
        const auto client = static_cast<const ZkClient*>(data);
        client->created_.release();
        LOG_INFO << "stringCompletion done...";
    }

}