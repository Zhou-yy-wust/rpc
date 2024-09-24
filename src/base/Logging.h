//
// Created by blair on 2024/9/22.
//

#ifndef LOGGING_H
#define LOGGING_H

#include "Timestamp.h"
#include "LogStream.h"

#include <filesystem>
#include <functional>

namespace rpc::base
{
class Logger
{
public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS
    };

    Logger(const std::filesystem::path& file, int line);
    Logger(const std::filesystem::path& file, int line, LogLevel level);
    Logger(const std::filesystem::path& file, int line, LogLevel level, const char* func);
    Logger(const std::filesystem::path& file, int line, bool toAbort);
    ~Logger();

    LogStream& stream();
    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    using OutputFunc = std::function<void(std::string_view)>;
    using FlushFunc = std::function<void()>;
    static void setOutput(OutputFunc);   // 回调
    static void setFlush(FlushFunc);  // 回调

private:
    class Impl
    {
    public:
        Impl(LogLevel level, int old_errno, const std::filesystem::path& file, int line);
        void formatTime();
        void finish();

        Timestamp time_;
        LogStream stream_;
        LogLevel level_;
        int line_;
        std::string file_;
    };
    Impl impl_;
};


    extern Logger::LogLevel g_logLevel;
    inline Logger::LogLevel Logger::logLevel(){return g_logLevel;}
    inline LogStream &Logger::stream(){return impl_.stream_;}

#define LOG_TRACE if (rpc::base::Logger::logLevel() <= rpc::base::Logger::TRACE) \
rpc::base::Logger(__FILE__, __LINE__, rpc::base::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (rpc::base::Logger::logLevel() <= rpc::base::Logger::DEBUG) \
rpc::base::Logger(__FILE__, __LINE__, rpc::base::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (rpc::base::Logger::logLevel() <= rpc::base::Logger::INFO) \
rpc::base::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN rpc::base::Logger(__FILE__, __LINE__, rpc::base::Logger::WARN).stream()
#define LOG_ERROR rpc::base::Logger(__FILE__, __LINE__, rpc::base::Logger::ERROR).stream()
#define LOG_FATAL rpc::base::Logger(__FILE__, __LINE__, rpc::base::Logger::FATAL).stream()
#define LOG_SYSERR rpc::base::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL rpc::base::Logger(__FILE__, __LINE__, true).stream()
}

#endif //LOGGING_H
