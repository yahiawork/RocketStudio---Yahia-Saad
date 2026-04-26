#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

#include "../physics/Constants.h"

namespace rs::math {

inline double clamp(double value, double minValue, double maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}

inline double degToRad(double degrees) {
    return degrees * constants::pi / 180.0;
}

inline double radToDeg(double radians) {
    return radians * 180.0 / constants::pi;
}

inline bool isFinite(double value) {
    return std::isfinite(value);
}

inline double safePositive(double value, double fallback) {
    return value > 0.0 && std::isfinite(value) ? value : fallback;
}

inline double niceGridStep(double visibleMeters, int targetLines) {
    if (visibleMeters <= 0.0 || targetLines <= 0) {
        return 1000.0;
    }

    const double raw = visibleMeters / static_cast<double>(targetLines);
    const double exponent = std::floor(std::log10(raw));
    const double base = std::pow(10.0, exponent);
    const double normalized = raw / base;

    if (normalized < 1.5) {
        return 1.0 * base;
    }
    if (normalized < 3.5) {
        return 2.0 * base;
    }
    if (normalized < 7.5) {
        return 5.0 * base;
    }
    return 10.0 * base;
}

} // namespace rs::math
