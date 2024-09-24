//
// Created by blair on 2024/9/21.
//

#ifndef LOGFILE_H
#define LOGFILE_H

#include <cstdio>
#include <memory>
#include <mutex>
#include <string>
#include <fstream>

#include "noncopyable.h"
#include "Timestamp.h"

namespace rpc::base
{
    class AppendFile: noncopyable
    {
        // 流缓冲区机制，减少系统调用次数，提升效率
    public:
        explicit AppendFile(const std::string&);
        ~AppendFile();
        void write(std::string_view s);
        void flush() const;  // 刷新缓冲区
        off_t writtenBytes() const;  // NOLINT(*-use-nodiscard)

    private:
        FILE* fp_;  // 管理的文件
        char buffer_[64 * 1024]{}; // 文件的缓冲区
        off_t writtenBytes_{0};  // 已经写入的字节数
    };


    class LogFile: noncopyable
    {
    public:
        LogFile(
            const std::string&basename,
            off_t rollSize,
            bool threadSafe,
            int flushInterval = 3,
            int checkEveryN = 1024);
        ~LogFile() = default;

        void write(std::string_view logline);
        void flush() const;
        bool rollFile();
    private:
        void write_unlocked(std::string_view logline);
        static std::string getLogFileName(const std::string&basename, std::time_t& now);

        const std::string basename_;  // 基础名字
        const off_t rollSize_;  // 每个日志文件的最大大小，超过此大小则滚动一次
        const int flushInterval_;  // 刷新间隔
        const int checkEveryN_;   // 每N次写，检查是否需要滚动和刷新

        int count;

        std::unique_ptr<std::mutex> mutex_;
        std::time_t startOfPeriod_;
        std::time_t lastRoll_;
        std::time_t lastFlush_;
        std::unique_ptr<AppendFile> file_;

        static constexpr int kRollPerSeconds_ = 60 * 60 * 24;   // 每24小时滚动一次
    };
}
#endif //LOGFILE_H
