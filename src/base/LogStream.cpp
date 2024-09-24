//
// Created by blair on 2024/9/21.
//

#include "LogStream.h"

#include <cassert>

using namespace rpc::base;
void LogStream::write(std::string_view data){buffer_.write(data);}
void LogStream::clear(){buffer_.clear();}
const LogStream::Buffer& LogStream::buffer() const{return buffer_;}


template<typename T>
Fmt::Fmt(const char* fmt, T val)
{
    static_assert(std::is_arithmetic_v<T>, "Must be arithmetic type");

    length_ = snprintf(buf_, sizeof buf_, fmt, val);
    assert(static_cast<size_t>(length_) < sizeof buf_);
}

// Explicit instantiations
template Fmt::Fmt(const char* fmt, char);
template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);

template Fmt::Fmt(const char* fmt, float);
template Fmt::Fmt(const char* fmt, double);

const char* Fmt::data() const {return buf_;}
int Fmt::length() const {return length_;}


LogStream& operator<<(LogStream& s, const Fmt& fmt)
{
    s.write(fmt.data());
    return s;
}
