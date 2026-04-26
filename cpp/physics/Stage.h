#pragma once

#include <string>

namespace rs {

struct Stage {
    std::string name = "Stage";
    double dryMass = 1000.0;
    double fuelMass = 1000.0;
    double thrust = 100000.0;
    double massFlowRate = 10.0;
    double burnTime = 100.0;
    double separationDelay = 1.0;
    double cdMultiplier = 1.0;
    double areaMultiplier = 1.0;
};

} // namespace rs
