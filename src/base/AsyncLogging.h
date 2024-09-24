//
// Created by blair on 2024/9/21.
//

#ifndef ASYNCLOGGING_H
#define ASYNCLOGGING_H

#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

#include "LogStream.h"
#include "noncopyable.h"

namespace rpc::base
{
    class CountDownLatch: noncopyable
    {
    public:
        explicit CountDownLatch(int count);
        void wait();
        void countDown();
        int getCount() const;
    private:
        std::condition_variable cv_;
        mutable std::mutex mutex_;
        int count_;
    };


    class AsyncLogging: noncopyable
    {
    public:
        AsyncLogging(std::string basename, off_t rollSize, int flushInterval=3);
        ~AsyncLogging();

        void write(std::string_view logline);
        void start();
        void stop();
    private:
        void threadFunc();
        using Buffer = FixedBuffer<kLargeBuffer>;
        using BufferVector = std::vector<std::unique_ptr<Buffer>>;
        using BufferPtr = BufferVector::value_type;

        std::thread thread_;
        std::atomic<bool> running_{false};
        CountDownLatch latch_;

        const int flushInterval_;
        const std::string basename_;
        const off_t rollSize_;

        std::mutex mutex_;
        std::condition_variable cv_;  // GUARDED_BY(mutex_);
        BufferVector buffers_;  // GUARDED_BY(mutex_);
        BufferPtr currentBuffer_; // GUARDED_BY(mutex_);
        BufferPtr nextBuffer_;  // GUARDED_BY(mutex_);

    };
}

#endif //ASYNCLOGGING_H
