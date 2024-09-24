//
// Created by blair on 2024/9/22.
//

#include "Logging.h"
#include <thread>
#include <utility>

namespace rpc::base
{
    thread_local std::string t_time;
    thread_local time_t t_lastSecond;
    thread_local char t_errnobuf[512];

    const char* strerror_tl(int savedErrno)
    {
        return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
    }

    const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
    {
        "TRACE ",
        "DEBUG ",
        "INFO  ",
        "WARN  ",
        "ERROR ",
        "FATAL ",
      };

    Logger::LogLevel initLogLevel()
    {
        if(::getenv("RPC_LOR_TRANCE"))
            return Logger::TRACE;
        if(::getenv("RPC_LOG_DEGUG"))
            return Logger::DEBUG;
        return Logger::INFO;
    }

    void defaultOutput(std::string_view msg)
    {
        fwrite(msg.begin(), 1, msg.size(), stdout);
    }

    void defaultFlush()
    {
        fflush(stdout);
    }

    Logger::LogLevel g_logLevel = initLogLevel();
    Logger::OutputFunc g_output = defaultOutput;
    Logger::FlushFunc g_flush = defaultFlush;


    // imlp
    Logger::Impl::Impl(
        LogLevel level,
        int savedErrno,
        const std::filesystem::path &file,
        int line
        )
        : time_(Timestamp::now())
        , level_(level)
        , line_(line)
        , file_(file.filename())
    {
        formatTime();
        // std::ostringstream oss;
        // oss << std::this_thread::get_id();
        // stream_ << oss.str();
        stream_  << LogLevelName[level];
        if (savedErrno != 0)
        {
            stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
        }
    }


    void Logger::Impl::formatTime()
    {
        auto microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
        auto seconds = static_cast<time_t>(microSecondsSinceEpoch / 1000000);
        if(seconds != t_lastSecond)
        {
            t_lastSecond = seconds;
            auto dt = std::gmtime(&seconds);
            std::ostringstream oss;
            oss << std::put_time(dt, "%Y%m%d %H:%M:%S");
            t_time = oss.str();
        }
        stream_ << t_time << " ";
    }


    void Logger::Impl::finish()
    {
        stream_ << " - " << file_ << ':' << line_ << '\n';
    }

    Logger::Logger(const std::filesystem::path& file, int line)
        : impl_(INFO, 0, file, line){}

    Logger::Logger(const std::filesystem::path& file, int line, LogLevel level, const char *func)
        : impl_(level, 0, file, line)
    {
        impl_.stream_ << func << ' ';
    }

    Logger::Logger(const std::filesystem::path& file, int line, LogLevel level)
        : impl_(level, 0, file, line){}


    Logger::Logger(const std::filesystem::path& file, int line, bool toAbort)
        :impl_(toAbort? FATAL: ERROR, errno, file, line){}

    Logger::~Logger()
    {
        impl_.finish();
        const LogStream::Buffer& buf(stream().buffer());
        g_output({buf.cbegin(), buf.size()});
        if(impl_.level_ == FATAL)
        {
            g_flush();
            abort();
        }
    }

    void Logger::setLogLevel(LogLevel level)
    {
        g_logLevel = level;
    }

    void Logger::setOutput(OutputFunc out)
    {
        g_output = std::move(out);
    }

    void Logger::setFlush(FlushFunc flush)
    {
        g_flush = std::move(flush);
    }
}
