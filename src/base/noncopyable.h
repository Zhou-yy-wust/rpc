//
// Created by blair on 2024/9/21.
//

#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

namespace rpc::base
{
    class noncopyable
    {
    public:
        noncopyable(const noncopyable&) = delete;
        noncopyable& operator=(const noncopyable&) = delete;
    protected:
        noncopyable() = default;   // 不希望这个类本身被实例化，因此将析构和构造声明为protected
        ~noncopyable() = default;
    };
}

#endif //NONCOPYABLE_H
