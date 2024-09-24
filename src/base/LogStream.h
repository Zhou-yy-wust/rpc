//
// Created by blair on 2024/9/21.
//

#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include <cstdint>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <type_traits>

#include "noncopyable.h"
#include "FixedBuffer.h"

namespace rpc::base
{
    class LogStream : noncopyable
    {
    public:
        using Buffer = FixedBuffer<kSmallBuffer>;
        /*
         * 定义的 operator<< 重载函数，这些函数处理不同类型的数据输入，并将它们格式化后追加到 buffer_ 缓冲区中。
         * 重载涵盖了指针类型、字符数组、字符串、浮点数、整数、布尔值和字符类型，确保 self 类能够灵活处理不同的数据类型并高效输出。
         */
        // 指针类型
        template<typename T, std::enable_if_t<std::is_pointer_v<T>, int> = 0>
        LogStream& operator<<(T t)
        {
            const auto v = reinterpret_cast<uintptr_t>(t);
            if(buffer_.avail() >= kMaxNumericSize)
            {
                std::ostringstream oss;
                oss << "0x" << std::hex << v;
                const std::string hexString = oss.str();
                buffer_.write(hexString);
            }
            return *this;
        }

        // 字符数组，string
        LogStream& operator<<(const char* str)
        {
            if (str) buffer_.write(str);
            else buffer_.write("(null)");
            return *this;
        }

        LogStream& operator<<(const unsigned char* str)
        {
            return operator<<(reinterpret_cast<const char*>(str));
        }

        LogStream& operator<<(const std::string& str)
        {
            buffer_.write(str);
            return *this;
        }

        LogStream& operator<<(std::string_view str)
        {
            buffer_.write(str);
            return *this;
        }


        // 浮点数类型
        template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
        LogStream& operator<<(T t)
        {
            if(buffer_.avail() >= kMaxNumericSize)
            {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(12) << t;  // 固定浮点数格式并设置精度
                const std::string floatString = oss.str();
                buffer_.write(floatString);  // 将转换后的浮点数字符串追加到缓冲区
            }
            return *this;
        }

        // 整数类型
        template<typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
        LogStream& operator<<(T t)
        {
            if(buffer_.avail() >= kMaxNumericSize)
                buffer_.write(std::to_string(t));
            return *this;
        }

        LogStream& operator<<(const bool t)
        {
            buffer_.write(t? "1": "0");
            return *this;
        }

        LogStream& operator<<(const char t)
        {
            buffer_.write(&t);
            return *this;
        }

        // 接口
        void write(std::string_view data);
        void clear();
        const Buffer& buffer() const; // NOLINT(*-use-nodiscard)
    private:
        Buffer buffer_;
        static constexpr int kMaxNumericSize = 48;
    };


    class Fmt
    {
    public:
        template<typename T>
        Fmt(const char* fmt, T val);

        const char* data() const; // NOLINT(*-use-nodiscard)
        int length() const; // NOLINT(*-use-nodiscard)
    private:
        char buf_[32]{};
        int length_;
    };


    LogStream& operator<<(LogStream& s, const Fmt& fmt);
}
#endif //LOGSTREAM_H
