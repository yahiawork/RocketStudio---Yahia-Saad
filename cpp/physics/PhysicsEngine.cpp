#include "PhysicsEngine.h"

#include <algorithm>
#include <cmath>

#include "../utils/MathUtils.h"
#include "Environment.h"

namespace rs {

const char* simulationStatusName(SimulationStatus status) {
    switch (status) {
        case SimulationStatus::Stopped:
            return "stopped";
        case SimulationStatus::Running:
            return "running";
        case SimulationStatus::Paused:
            return "paused";
        case SimulationStatus::Finished:
            return "finished";
    }
    return "unknown";
}

PhysicsEngine::PhysicsEngine() {
    reset(SimulationConfig{});
}

PhysicsEngine::PhysicsEngine(const SimulationConfig& config) {
    reset(config);
}

void PhysicsEngine::applyConfig(const SimulationConfig& config) {
    config_ = config;
    sanitizeConfig();
}

SimulationConfig& PhysicsEngine::editableConfig() {
    return config_;
}

const SimulationConfig& PhysicsEngine::config() const {
    return config_;
}

void PhysicsEngine::reset() {
    reset(config_);
}

void PhysicsEngine::reset(const SimulationConfig& config) {
    config_ = config;
    sanitizeConfig();
    rocket_.reset(config_);

    const double angle = math::degToRad(config_.initialAngleDeg);
    state_ = PhysicsState{};
    state_.time = 0.0;
    state_.position = {config_.initialDownrange, std::max(0.0, config_.initialAltitude)};
    state_.velocity = {std::cos(angle) * config_.initialVelocity, std::sin(angle) * config_.initialVelocity};
    state_.acceleration = {};

    recorder_.clear();
    accumulator_ = 0.0;
    hasLaunched_ = state_.position.y > 1.0 || state_.velocity.length() > 1.0;
    status_ = SimulationStatus::Stopped;
    recordCurrentSample();
}

void PhysicsEngine::start() {
    if (status_ == SimulationStatus::Finished || status_ == SimulationStatus::Stopped) {
        reset(config_);
    }
    status_ = SimulationStatus::Running;
}

void PhysicsEngine::pause() {
    if (status_ == SimulationStatus::Running) {
        status_ = SimulationStatus::Paused;
    }
}

void PhysicsEngine::resume() {
    if (status_ == SimulationStatus::Paused) {
        status_ = SimulationStatus::Running;
    }
}

void PhysicsEngine::stop() {
    status_ = SimulationStatus::Stopped;
}

void PhysicsEngine::stepOnce() {
    if (status_ == SimulationStatus::Finished) {
        return;
    }
    integrateOneStep(config_.dt);
    if (shouldFinish()) {
        status_ = SimulationStatus::Finished;
    }
}

void PhysicsEngine::update(double realDeltaSeconds) {
    if (status_ != SimulationStatus::Running) {
        return;
    }

    const double scaled = math::clamp(realDeltaSeconds, 0.0, 0.25) * config_.timeScale;
    accumulator_ += scaled;

    int steps = 0;
    constexpr int maxStepsPerFrame = 250;
    while (accumulator_ >= config_.dt && steps < maxStepsPerFrame && status_ == SimulationStatus::Running) {
        integrateOneStep(config_.dt);
        accumulator_ -= config_.dt;
        ++steps;

        if (shouldFinish()) {
            status_ = SimulationStatus::Finished;
        }
    }

    if (steps == maxStepsPerFrame) {
        accumulator_ = 0.0;
    }
}

void PhysicsEngine::runToEnd() {
    if (status_ == SimulationStatus::Stopped) {
        start();
    }
    int guard = 0;
    const int maxGuardSteps = static_cast<int>(std::ceil(config_.maxTime / config_.dt)) + 1000;
    while (status_ == SimulationStatus::Running && guard < maxGuardSteps) {
        integrateOneStep(config_.dt);
        if (shouldFinish()) {
            status_ = SimulationStatus::Finished;
        }
        ++guard;
    }
    if (guard >= maxGuardSteps) {
        status_ = SimulationStatus::Finished;
    }
}

SimulationStatus PhysicsEngine::status() const {
    return status_;
}

const PhysicsState& PhysicsEngine::state() const {
    return state_;
}

const SimulationRecorder& PhysicsEngine::recorder() const {
    return recorder_;
}

const Rocket& PhysicsEngine::rocket() const {
    return rocket_;
}

ForceBreakdown PhysicsEngine::evaluateForces(const PhysicsState& state, double time) const {
    ForceBreakdown forces;
    const double altitude = std::max(0.0, state.position.y);
    const double speed = state.velocity.length();

    forces.mass = std::max(1.0, rocket_.totalMass());
    forces.fuelRemaining = rocket_.fuelRemaining();
    forces.currentStage = rocket_.currentStageIndex();

    const double pitchRad = math::degToRad(pitchAngleDeg(time));
    const Vec2 thrustDirection{std::cos(pitchRad), std::sin(pitchRad)};
    forces.thrust = rocket_.currentThrust();
    forces.thrustForce = thrustDirection * forces.thrust;

    forces.gravity = Environment::gravityAtAltitude(altitude);
    forces.gravityForce = {0.0, -forces.gravity * forces.mass};

    forces.density = Environment::densityAtAltitude(altitude, config_.atmosphereEnabled);
    forces.dynamicPressure = Environment::dynamicPressure(forces.density, speed);

    const double cd = std::max(0.0, config_.baseCd * rocket_.activeCdMultiplier());
    const double area = std::max(0.0, config_.baseArea * rocket_.activeAreaMultiplier());
    forces.drag = Environment::dragMagnitude(forces.density, speed, cd, area);
    if (speed > 1.0e-9 && forces.drag > 0.0) {
        forces.dragForce = state.velocity.normalized() * (-forces.drag);
    }

    const Vec2 netForce = forces.thrustForce + forces.gravityForce + forces.dragForce;
    forces.acceleration = netForce / forces.mass;
    forces.escapeVelocity = Environment::escapeVelocity(altitude);
    return forces;
}

double PhysicsEngine::pitchAngleDeg(double time) const {
    if (time <= config_.pitchStartTime) {
        return math::clamp(config_.initialAngleDeg, 0.0, 90.0);
    }

    const double elapsed = time - config_.pitchStartTime;
    const double angle = config_.initialAngleDeg - elapsed * config_.pitchRateDegPerSec;
    return math::clamp(angle, config_.minPitchDeg, 90.0);
}

void PhysicsEngine::sanitizeConfig() {
    config_.dt = math::clamp(config_.dt, 0.001, 2.0);
    config_.maxTime = math::clamp(config_.maxTime, 1.0, 20000.0);
    config_.timeScale = math::clamp(config_.timeScale, 0.05, 500.0);
    config_.initialAltitude = std::max(0.0, config_.initialAltitude);
    config_.initialVelocity = std::max(0.0, config_.initialVelocity);
    config_.initialAngleDeg = math::clamp(config_.initialAngleDeg, 0.0, 90.0);
    config_.payloadMass = std::max(1.0, config_.payloadMass);
    config_.baseCd = math::clamp(config_.baseCd, 0.0, 4.0);
    config_.baseArea = math::clamp(config_.baseArea, 0.01, 10000.0);
    config_.pitchStartTime = std::max(0.0, config_.pitchStartTime);
    config_.pitchRateDegPerSec = math::clamp(config_.pitchRateDegPerSec, 0.0, 5.0);
    config_.minPitchDeg = math::clamp(config_.minPitchDeg, 0.0, 90.0);

    if (config_.stages.empty()) {
        Stage stage;
        stage.name = "Single Stage";
        config_.stages.push_back(stage);
    }

    for (size_t i = 0; i < config_.stages.size(); ++i) {
        Stage& stage = config_.stages[i];
        if (stage.name.empty()) {
            stage.name = "Stage " + std::to_string(i + 1);
        }
        stage.dryMass = std::max(0.0, stage.dryMass);
        stage.fuelMass = std::max(0.0, stage.fuelMass);
        stage.thrust = std::max(0.0, stage.thrust);
        stage.massFlowRate = std::max(0.0, stage.massFlowRate);
        stage.burnTime = std::max(0.0, stage.burnTime);
        stage.separationDelay = std::max(0.0, stage.separationDelay);
        stage.cdMultiplier = math::clamp(stage.cdMultiplier, 0.05, 10.0);
        stage.areaMultiplier = math::clamp(stage.areaMultiplier, 0.05, 10.0);
    }
}

void PhysicsEngine::recordCurrentSample() {
    const ForceBreakdown forces = evaluateForces(state_, state_.time);
    state_.acceleration = forces.acceleration;
    const bool escapeNow = state_.velocity.length() >= forces.escapeVelocity;
    recorder_.recordSample(sampleFromBreakdown(forces), escapeNow);
}

void PhysicsEngine::integrateOneStep(double dt) {
    const DerivativeFunction derivative = [this](const PhysicsState& state, double time) {
        const ForceBreakdown forces = evaluateForces(state, time);
        StateDerivative result;
        result.dPosition = state.velocity;
        result.dVelocity = forces.acceleration;
        return result;
    };

    if (config_.integrator == IntegratorType::Euler) {
        state_ = Integrator::euler(state_, dt, derivative);
    } else {
        state_ = Integrator::rk4(state_, dt, derivative);
    }

    if (state_.position.y > 1.0 || state_.velocity.length() > 1.0) {
        hasLaunched_ = true;
    }

    if (state_.position.y < 0.0) {
        state_.position.y = 0.0;
    }

    const std::vector<StageEvent> events =
        rocket_.advanceAfterPhysicsStep(dt, state_.time, state_.position, state_.velocity.length());
    for (const StageEvent& event : events) {
        recorder_.recordStageEvent(event);
    }

    recordCurrentSample();
}

TelemetrySample PhysicsEngine::sampleFromBreakdown(const ForceBreakdown& forces) const {
    TelemetrySample sample;
    sample.time = state_.time;
    sample.x = state_.position.x;
    sample.altitude = state_.position.y;
    sample.vx = state_.velocity.x;
    sample.vy = state_.velocity.y;
    sample.speed = state_.velocity.length();
    sample.ax = forces.acceleration.x;
    sample.ay = forces.acceleration.y;
    sample.acceleration = forces.acceleration.length();
    sample.mass = forces.mass;
    sample.fuelRemaining = forces.fuelRemaining;
    sample.thrust = forces.thrust;
    sample.drag = forces.drag;
    sample.gravity = forces.gravity;
    sample.density = forces.density;
    sample.dynamicPressure = forces.dynamicPressure;
    sample.currentStage = forces.currentStage;
    return sample;
}

bool PhysicsEngine::shouldFinish() const {
    if (!stateIsFinite()) {
        return true;
    }
    if (state_.time >= config_.maxTime) {
        return true;
    }
    if (hasLaunched_ && state_.position.y <= 0.0 && state_.velocity.y < -0.1 && state_.time > 1.0) {
        return true;
    }
    return false;
}

bool PhysicsEngine::stateIsFinite() const {
    return math::isFinite(state_.time) &&
           math::isFinite(state_.position.x) &&
           math::isFinite(state_.position.y) &&
           math::isFinite(state_.velocity.x) &&
           math::isFinite(state_.velocity.y);
}

} // namespace rs
