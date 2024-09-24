//
// Created by blair on 2024/9/21.
//

#include "AsyncLogging.h"
#include "LogFile.h"

#include <memory>
#include <utility>
#include <cassert>

using namespace rpc::base;

CountDownLatch::CountDownLatch(int count): count_(count){}

void CountDownLatch::wait()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this](){return count_ <= 0;});
}

int CountDownLatch::getCount() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return count_;
}

void CountDownLatch::countDown()
{
    std::lock_guard<std::mutex> lock(mutex_);
    --count_;
    if(count_ <= 0)
        cv_.notify_all();
}



AsyncLogging::AsyncLogging(
    std::string  basename,
    off_t rollSize,
    int flushInterval
    )
    : latch_(1)
    , flushInterval_(flushInterval)
    , basename_(std::move(basename))
    , rollSize_(rollSize)
    , currentBuffer_(new Buffer)
    , nextBuffer_(new Buffer)
{
    buffers_.reserve(16);
}


AsyncLogging::~AsyncLogging()
{
    if(running_) stop();
}



void AsyncLogging::write(std::string_view logline)
{
    std::lock_guard lock(mutex_);
    if(currentBuffer_->avail() > logline.size())
        currentBuffer_->write(logline);
    else
    {
        buffers_.push_back(std::move(currentBuffer_));
        if(nextBuffer_) currentBuffer_ = std::move(nextBuffer_);
        else currentBuffer_ = std::make_unique<Buffer>();
        currentBuffer_->write(logline);
        cv_.notify_one();
    }
}


void AsyncLogging::start()
{
    running_ = true;
    thread_ = std::thread([this] { threadFunc(); });
    latch_.wait();  // 阻塞等待线程启动
}


void AsyncLogging::stop()
{
    running_ = false;
    cv_.notify_one();
    thread_.join(); // 阻塞等待线程join
}


void AsyncLogging::threadFunc()
{
    latch_.countDown();
    LogFile output(basename_, rollSize_, false);
    BufferPtr newBuffer1(new Buffer), newBuffer2(new Buffer);
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);

    while(running_)
    {
        assert(newBuffer1 && newBuffer1->empty());
        assert(newBuffer2 && newBuffer2->empty());
        assert(buffersToWrite.empty());

        {
            std::unique_lock<std::mutex> lock(mutex_);
            if(buffers_.empty())
                cv_.wait_for(lock, std::chrono::seconds(flushInterval_));

            // currentBuffer_进入buffers，确保一次性交换完全部数据
            buffers_.push_back(std::move(currentBuffer_));
            currentBuffer_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if(!nextBuffer_) nextBuffer_ = std::move(newBuffer2);
        }

        assert(!buffersToWrite.empty());
        if(buffersToWrite.size() > 25)
        {
            char buf[256];
            snprintf(buf, sizeof buf, "Dropped log message at %s, %zd larger buffers \n",
                Timestamp::now().toFormattedString().c_str(),
                buffersToWrite.size() - 2);
            fputs(buf, stderr);
            output.write(buf);
            buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        }

        for(const auto& buffer: buffersToWrite)
            output.write(buffer->cbegin());

        if(buffersToWrite.size() > 2) buffersToWrite.resize(2);
        if(!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            //newBuffer1.reset();
        }
        if(!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            //newBuffer2.reset();
        }
        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}


