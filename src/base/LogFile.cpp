//
// Created by blair on 2024/9/21.
//

#include "LogFile.h"

#include <cassert>
#include <unistd.h>
#include <cstddef>
#include <cstring>
#include <iomanip>
#include <iostream>

namespace rpc::base
{
    static thread_local char t_errnobuf[512];

    AppendFile::AppendFile(const std::string & filename)
        :fp_(fopen(filename.c_str(), "ae"))
    {
        assert(fp_);
        setbuffer(fp_, buffer_,  sizeof buffer_);
    }

    AppendFile::~AppendFile(){fclose(fp_);}


    void AppendFile::write(std::string_view logline)
    {
        size_t written = 0;
        const size_t len = logline.size();

        while(written != len)
        {
            const size_t remain = len - written;
            const size_t n = ::fwrite_unlocked(logline.data() + written, 1, remain, fp_);
            if(n != remain)
            {
                if(const int err = ferror(fp_))
                {
                    std::cerr << "AppendFile::append() failed "
                        << strerror_r(err, t_errnobuf, sizeof t_errnobuf) << std::endl;
                    break;
                }
            }
            written += n;
        }
        writtenBytes_ += static_cast<off_t>(written);
    }

    void AppendFile::flush() const {fflush(fp_);}

    off_t AppendFile::writtenBytes() const{ return writtenBytes_; }



    LogFile::LogFile(
        const std::string &basename,
        off_t rollSize,
        bool threadSafe,
        int flushInterval,
        int checkEveryN
        )
    : basename_(basename)
    , rollSize_(rollSize)
    , flushInterval_(flushInterval)
    , checkEveryN_(checkEveryN)
    , count(0)
    , mutex_(threadSafe? new std::mutex: nullptr)
    , startOfPeriod_(0)
    , lastRoll_(0)
    , lastFlush_(0)
    {
        assert(basename.find('/') == std::string::npos);
        rollFile();
    }

    void LogFile::write(std::string_view logline)
    {
        if(mutex_)
        {
            std::lock_guard<std::mutex> lock(*mutex_);
            write_unlocked(logline);
        }
        else
            write_unlocked(logline);
    }

    void LogFile::flush() const
    {
        if(mutex_)
        {
            std::lock_guard<std::mutex> lock(*mutex_);
            file_->flush();
        }
        else file_->flush();

    }

    void LogFile::write_unlocked(std::string_view logline)
    {
        file_->write(logline);
        if(file_->writtenBytes() > rollSize_) rollFile();
        else
        {
            ++count;
            if(count >= checkEveryN_)
            {
                count = 0;
                std::time_t now = time(nullptr);
                std::time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;
                if(thisPeriod_ != startOfPeriod_) rollFile();
                else if(now - lastFlush_ > flushInterval_)
                {
                    lastFlush_ = now;
                    file_->flush();
                }
            }
        }
    }


    bool LogFile::rollFile()
    {
        std::time_t now = 0;
        auto filename = getLogFileName(basename_, now);
        std::time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;
        if(now > lastRoll_)
        {
            lastRoll_ = now, lastFlush_ = now;
            startOfPeriod_ = start;
            file_ = std::make_unique<AppendFile>(filename);
            return true;
        }
        return false;
    }


    std::string LogFile::getLogFileName(const std::string &basename, std::time_t& now)
    {
        std::ostringstream filename;
        filename << basename;
        now = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        std::tm localtime = *std::localtime(&now);
        filename << std::put_time(&localtime, "%Y%m%d=%H%M%S");
        filename << std::to_string(getpid()) << ".log";
        return filename.str();
    }
}


