//
// Created by blair on 2024/9/21.
//


#include "rpccontroller.h"

namespace rpc
{
    void RpcController::Reset()
    {
        failed_ = false;
        errmsg_ = "";
    }

    bool RpcController::Failed() const{return failed_;}

    std::string RpcController::ErrorText() const{return errmsg_;}

    void RpcController::SetFailed(const std::string &reason)
    {
        failed_ = true;
        errmsg_ = reason;
    }


    void RpcController::StartCancel(){}

    bool RpcController::IsCanceled() const{return false;}

    void RpcController::NotifyOnCancel(google::protobuf::Closure *callback){}
}




