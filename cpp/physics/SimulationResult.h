#pragma once

#include <string>
#include <vector>

namespace rs {

struct TelemetrySample {
    double time = 0.0;
    double x = 0.0;
    double altitude = 0.0;
    double vx = 0.0;
    double vy = 0.0;
    double speed = 0.0;
    double ax = 0.0;
    double ay = 0.0;
    double acceleration = 0.0;
    double mass = 0.0;
    double fuelRemaining = 0.0;
    double thrust = 0.0;
    double drag = 0.0;
    double gravity = 0.0;
    double density = 0.0;
    double dynamicPressure = 0.0;
    int currentStage = -1;
};

struct StageEvent {
    std::string name;
    int stageIndex = -1;
    double time = 0.0;
    double x = 0.0;
    double altitude = 0.0;
    double velocity = 0.0;
};

struct SimulationSummary {
    double maxAltitude = 0.0;
    double maxAltitudeTime = 0.0;
    double maxVelocity = 0.0;
    double maxVelocityTime = 0.0;
    double maxAcceleration = 0.0;
    double maxQ = 0.0;
    double maxQTime = 0.0;
    double maxQAltitude = 0.0;
    double maxQVelocity = 0.0;
    double finalTime = 0.0;
    double finalAltitude = 0.0;
    double finalVelocity = 0.0;
    bool escapeVelocityReached = false;
    std::vector<StageEvent> stageEvents;
};

} // namespace rs
