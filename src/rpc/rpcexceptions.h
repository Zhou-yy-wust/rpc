//
// Created by blair on 2024/9/21.
//

#ifndef RPCEXCEPTIONS_H
#define RPCEXCEPTIONS_H
#include <exception>
#include <string>

namespace rpc
{
    class ExceptionBase: public std::exception
    {
    public:
        ExceptionBase(const std::string& message, const char* file, int line);
        const char* what() const noexcept override;
    private:
        mutable std::string full_message_;
        const char* file_;
        int line_;
    };


    class SerializeError final: public ExceptionBase
    {
    public:
        using ExceptionBase::ExceptionBase;
    };


    class ParseError final: public ExceptionBase
    {
    public:
        using ExceptionBase::ExceptionBase;
    };


    class FindMethodError final: public ExceptionBase
    {
    public:
        using ExceptionBase::ExceptionBase;
    };


    class FindServiceError final: public ExceptionBase
    {
    public:
        using ExceptionBase::ExceptionBase;
    };
}

#endif //RPCEXCEPTIONS_H
