//
// Created by blair on 2024/9/22.
//

#ifndef FIXEDBUFFER_H
#define FIXEDBUFFER_H

#include <cstring>
#include <string_view>

#include "noncopyable.h"

namespace rpc::base
{
    constexpr int kSmallBuffer = 4000;
    constexpr int kLargeBuffer = 4000 * 1000;

    template <int SIZE>
    class FixedBuffer: noncopyable
    {
    public:
        using iterator = char *;
        using const_iterator = const char *;

        // 默认 构造和和析构
        FixedBuffer() {bzero();};
        ~FixedBuffer() = default;

        // 迭代器
        iterator begin() {return data_;}
         const_iterator cbegin() const {return data_;}
        iterator end() {return cur_;}
         const_iterator cend() const {return cur_;}

        // 容量
         std::size_t empty() const {return cur_ == data_;}
         std::size_t capacity() const {return sizeof data_;}
         std::size_t size() const {return static_cast<std::size_t>(cur_ - data_);}
         std::size_t avail() const {return static_cast<std::size_t>(end() - cur_);};

        // 修改器
        void clear() {cur_ = data_;bzero();}
        void write(std::string_view bytes);

    private:
        void bzero() {memset(data_, 0, SIZE);}
        const char* end() const {return data_ + sizeof data_;}
        char data_[SIZE]{};
        char* cur_ = data_;
    };

    template<int SIZE>
    void FixedBuffer<SIZE>::write(std::string_view bytes)
    {
        if(avail() > bytes.size())
        {
            memcpy(cur_, bytes.data(), bytes.size());
            cur_ += bytes.size();
        }
    }

}


#endif //FIXEDBUFFER_H
