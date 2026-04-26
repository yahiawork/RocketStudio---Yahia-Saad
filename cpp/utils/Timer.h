#pragma once

#include <chrono>

namespace rs {

class Timer {
public:
    Timer();
    double restart();
    double elapsed() const;

private:
    using Clock = std::chrono::steady_clock;
    Clock::time_point last_;
};

} // namespace rs
