//
// Created by blair on 2024/9/21.
//

#ifndef RPCCONTROLLER_H
#define RPCCONTROLLER_H

#include <google/protobuf/service.h>

namespace rpc
{
    class RpcController: public google::protobuf::RpcController
    {
    public:
        RpcController() = default;
        ~RpcController() override = default;

        void Reset() override;
        bool Failed() const override;
        std::string ErrorText() const override;
        void StartCancel() override;
        void SetFailed(const std::string &reason) override;
        bool IsCanceled() const override;
        void NotifyOnCancel(google::protobuf::Closure *callback) override;
    private:
        bool failed_{false};
        std::string errmsg_;
    };
}
#endif //RPCCONTROLLER_H
