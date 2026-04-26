#include "Environment.h"

#include <algorithm>
#include <cmath>

namespace rs {

double Environment::gravityAtAltitude(double altitudeMeters) {
    const double clampedAltitude = std::max(0.0, altitudeMeters);
    const double radius = constants::earthRadius + clampedAltitude;
    return constants::gravitationalConstant * constants::earthMass / (radius * radius);
}

double Environment::densityAtAltitude(double altitudeMeters, bool atmosphereEnabled) {
    if (!atmosphereEnabled) {
        return 0.0;
    }
    const double clampedAltitude = std::max(0.0, altitudeMeters);
    if (clampedAltitude >= constants::atmosphereLimitAltitude) {
        return 0.0;
    }
    return constants::seaLevelDensity * std::exp(-clampedAltitude / constants::atmosphereScaleHeight);
}

double Environment::dynamicPressure(double density, double speed) {
    return 0.5 * density * speed * speed;
}

double Environment::dragMagnitude(double density, double speed, double cd, double area) {
    if (speed <= 0.0 || density <= 0.0 || cd <= 0.0 || area <= 0.0) {
        return 0.0;
    }
    return dynamicPressure(density, speed) * cd * area;
}

double Environment::escapeVelocity(double altitudeMeters) {
    const double clampedAltitude = std::max(0.0, altitudeMeters);
    return std::sqrt(2.0 * constants::gravitationalConstant * constants::earthMass /
                     (constants::earthRadius + clampedAltitude));
}

} // namespace rs
