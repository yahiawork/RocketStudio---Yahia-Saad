#pragma once

#include <string>
#include <vector>

#include "Stage.h"

namespace rs {

enum class IntegratorType {
    Euler,
    RK4
};

inline const char* integratorName(IntegratorType type) {
    return type == IntegratorType::Euler ? "Euler" : "RK4";
}

struct SimulationConfig {
    std::string presetName = "Heavy Artemis-like Rocket";

    double dt = 0.02;
    double maxTime = 800.0;
    double timeScale = 1.0;

    double initialDownrange = 0.0;
    double initialAltitude = 0.0;
    double initialVelocity = 0.0;
    double initialAngleDeg = 90.0;

    double payloadMass = 10000.0;
    double baseCd = 0.35;
    double baseArea = 80.0;
    bool atmosphereEnabled = true;

    double pitchStartTime = 8.0;
    double pitchRateDegPerSec = 0.35;
    double minPitchDeg = 5.0;

    IntegratorType integrator = IntegratorType::RK4;
    std::vector<Stage> stages;
};

} // namespace rs
