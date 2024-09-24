
//
// Created by blair on 2024/9/23.
//

#ifndef ZKCLIENT_H
#define ZKCLIENT_H


#include <string>

#include <zookeeper.h>
#include <semaphore>


namespace rpc
{
    class ZkClient
    {
    public:
        ZkClient() = default;
        ~ZkClient(){if(zhandle_) zookeeper_close(zhandle_);};

        void start();
        void create(const char* path, const char*data, int len, int state=0);   // 创建znode
        std::string getData(const char* path);  // 获取znode
    private:
        zhandle_t* zhandle_{nullptr};
        mutable std::string result_;
        mutable std::binary_semaphore data_ready_{0};
        mutable std::binary_semaphore connected_{0};
        mutable std::binary_semaphore created_{0};
        static void dataCompletion(int rc, const char *value, int value_len, const Stat *stat, const void *data);
        static void watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
        static void stringCompletion(int rc, const char *value, const void *data);
    };
}
#endif //ZKCLIENT_H
