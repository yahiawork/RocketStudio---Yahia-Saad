#include "Timer.h"

namespace rs {

Timer::Timer() : last_(Clock::now()) {}

double Timer::restart() {
    const auto now = Clock::now();
    const std::chrono::duration<double> delta = now - last_;
    last_ = now;
    return delta.count();
}

double Timer::elapsed() const {
    const std::chrono::duration<double> delta = Clock::now() - last_;
    return delta.count();
}

} // namespace rs
