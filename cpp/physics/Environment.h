#pragma once

#include "Constants.h"

namespace rs {

class Environment {
public:
    static double gravityAtAltitude(double altitudeMeters);
    static double densityAtAltitude(double altitudeMeters, bool atmosphereEnabled = true);
    static double dynamicPressure(double density, double speed);
    static double dragMagnitude(double density, double speed, double cd, double area);
    static double escapeVelocity(double altitudeMeters);
};

} // namespace rs
