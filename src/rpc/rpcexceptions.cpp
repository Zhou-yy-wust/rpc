//
// Created by blair on 2024/9/21.
//


#include "rpcexceptions.h"

namespace rpc
{
    ExceptionBase::ExceptionBase(
        const std::string& message,
        const char* file,
        int line
        ): full_message_(message), file_(file), line_(line){}


    const char* ExceptionBase::what() const noexcept
    {
        full_message_ += " [File: " + std::string(file_) + ", Line: " + std::to_string(line_) + "]";
        return full_message_.c_str();
    }
}