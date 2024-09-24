//
// Created by blair on 2024/9/21.
//

#ifndef TIMESTAMP_H
#define TIMESTAMP_H
#include <chrono>
#include <string>


namespace rpc::base
{
    class Timestamp
    {
        // 定义时间戳，需要确定时间戳使用的时钟、duration，二者共同定义time_point
    public:
        using clock = std::chrono::system_clock;
        using microseconds = std::chrono::microseconds;
        using time_point = std::chrono::time_point<clock, microseconds>;

        Timestamp();
        explicit Timestamp(int64_t microSecondsSinceEpochArg);
        void swap(Timestamp& that) noexcept;
        std::string toString() const; // NOLINT(*-use-nodiscard)
        std::string toFormattedString(bool showMicroseconds = false) const; // NOLINT(*-use-nodiscard)
        bool valid() const; // NOLINT(*-use-nodiscard)
        int64_t microSecondsSinceEpoch() const; // NOLINT(*-use-nodiscard)
        std::time_t secondsSinceEpoch() const; // NOLINT(*-use-nodiscard)
        static Timestamp now();
        static Timestamp invalid();
        static Timestamp fromUnixTime(std::time_t t, int microseconds = 0);

    private:
        time_point time_;
    };


    inline bool operator<(const Timestamp& lhs, const Timestamp& rhs)
    {
        return lhs.microSecondsSinceEpoch()< rhs.microSecondsSinceEpoch();
    }

    inline bool operator==(const Timestamp& lhs, const Timestamp& rhs)
    {
        return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
    }

    inline double timeDifference(const Timestamp& lhs, const Timestamp& rhs)
    {
        return static_cast<double>(
            lhs.microSecondsSinceEpoch() - rhs.microSecondsSinceEpoch()) / 1000000.0;
    }


    inline Timestamp  addTime(const Timestamp& timestamp, double seconds)
    {
        auto delta = static_cast<int64_t>(seconds * 1000000);
        return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
    }
}

#endif //TIMESTAMP_H
