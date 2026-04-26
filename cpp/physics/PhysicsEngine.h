#pragma once

#include <string>

#include "Integrator.h"
#include "Rocket.h"
#include "SimulationRecorder.h"

namespace rs {

enum class SimulationStatus {
    Stopped,
    Running,
    Paused,
    Finished
};

const char* simulationStatusName(SimulationStatus status);

struct ForceBreakdown {
    Vec2 thrustForce;
    Vec2 gravityForce;
    Vec2 dragForce;
    Vec2 acceleration;
    double thrust = 0.0;
    double drag = 0.0;
    double gravity = 0.0;
    double density = 0.0;
    double dynamicPressure = 0.0;
    double mass = 1.0;
    double fuelRemaining = 0.0;
    double escapeVelocity = 0.0;
    int currentStage = -1;
};

class PhysicsEngine {
public:
    PhysicsEngine();
    explicit PhysicsEngine(const SimulationConfig& config);

    void applyConfig(const SimulationConfig& config);
    SimulationConfig& editableConfig();
    const SimulationConfig& config() const;

    void reset();
    void reset(const SimulationConfig& config);
    void start();
    void pause();
    void resume();
    void stop();
    void stepOnce();
    void update(double realDeltaSeconds);
    void runToEnd();

    SimulationStatus status() const;
    const PhysicsState& state() const;
    const SimulationRecorder& recorder() const;
    const Rocket& rocket() const;

    ForceBreakdown evaluateForces(const PhysicsState& state, double time) const;
    double pitchAngleDeg(double time) const;

private:
    SimulationConfig config_;
    Rocket rocket_;
    PhysicsState state_;
    SimulationRecorder recorder_;
    SimulationStatus status_ = SimulationStatus::Stopped;
    double accumulator_ = 0.0;
    bool hasLaunched_ = false;

    void sanitizeConfig();
    void recordCurrentSample();
    void integrateOneStep(double dt);
    TelemetrySample sampleFromBreakdown(const ForceBreakdown& forces) const;
    bool shouldFinish() const;
    bool stateIsFinite() const;
};

} // namespace rs
