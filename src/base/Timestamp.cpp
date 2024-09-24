//
// Created by blair on 2024/9/21.
//

#include "Timestamp.h"

#include <iomanip>
#include <sstream>

using namespace rpc::base;

Timestamp::Timestamp(): time_(time_point::min()){}

Timestamp::Timestamp(int64_t microSecondsSinceEpochArg)
: time_(time_point(microseconds(microSecondsSinceEpochArg)))
{}

void Timestamp::swap(Timestamp &that) noexcept
{
    std::swap(time_, that.time_);
}


std::string Timestamp::toString() const
{
    return std::to_string(microSecondsSinceEpoch());
}


std::string Timestamp::toFormattedString(bool showMicroseconds) const
{
    std::time_t timeT = clock::to_time_t(time_);
    std::tm tm = *std::localtime(&timeT);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    if(showMicroseconds)
    {
        auto microsecondsPart = microSecondsSinceEpoch() % 1000000;
        oss << "." << std::setw(6) << std::setfill('0') << microsecondsPart;
    }
    return oss.str();
}


bool Timestamp::valid() const {return time_ == time_point::min();}

int64_t Timestamp::microSecondsSinceEpoch() const
{
    return std::chrono::duration_cast<microseconds>(time_.time_since_epoch()).count();
}

std::time_t Timestamp::secondsSinceEpoch() const
{
    return std::chrono::duration_cast<std::chrono::seconds>(time_.time_since_epoch()).count();
}


Timestamp Timestamp::now()
{
    return Timestamp(
        std::chrono::duration_cast<microseconds>(
            clock::now().time_since_epoch()).count());
}


Timestamp Timestamp::invalid(){return {};}

Timestamp Timestamp::fromUnixTime(std::time_t t, int microseconds)
{
    return Timestamp(static_cast<int64_t>(t) * 1000000 + microseconds);
}





