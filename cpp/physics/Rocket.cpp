#include "Rocket.h"

#include <algorithm>

namespace rs {

Rocket::Rocket() {
    reset(SimulationConfig{});
}

Rocket::Rocket(const SimulationConfig& config) {
    reset(config);
}

void Rocket::reset(const SimulationConfig& config) {
    config_ = config;
    normalizeConfig();

    stageFuel_.clear();
    stageElapsed_.clear();
    stageFuel_.reserve(config_.stages.size());
    stageElapsed_.reserve(config_.stages.size());

    for (const Stage& stage : config_.stages) {
        stageFuel_.push_back(std::max(0.0, stage.fuelMass));
        stageElapsed_.push_back(0.0);
    }

    currentStageIndex_ = config_.stages.empty() ? -1 : 0;
    separationDelayRemaining_ = 0.0;
}

void Rocket::normalizeConfig() {
    config_.payloadMass = std::max(1.0, config_.payloadMass);
    if (config_.stages.empty()) {
        Stage fallback;
        fallback.name = "Single Stage";
        config_.stages.push_back(fallback);
    }

    for (Stage& stage : config_.stages) {
        stage.dryMass = std::max(0.0, stage.dryMass);
        stage.fuelMass = std::max(0.0, stage.fuelMass);
        stage.thrust = std::max(0.0, stage.thrust);
        stage.massFlowRate = std::max(0.0, stage.massFlowRate);
        stage.burnTime = std::max(0.0, stage.burnTime);
        stage.separationDelay = std::max(0.0, stage.separationDelay);
        stage.cdMultiplier = std::max(0.05, stage.cdMultiplier);
        stage.areaMultiplier = std::max(0.05, stage.areaMultiplier);
    }
}

double Rocket::totalMass() const {
    double mass = std::max(1.0, config_.payloadMass);

    if (currentStageIndex_ < 0) {
        return mass;
    }

    for (int i = currentStageIndex_; i < static_cast<int>(config_.stages.size()); ++i) {
        mass += config_.stages[static_cast<size_t>(i)].dryMass;
        mass += stageFuel_[static_cast<size_t>(i)];
    }

    return std::max(1.0, mass);
}

double Rocket::fuelRemaining() const {
    double fuel = 0.0;
    if (currentStageIndex_ < 0) {
        return fuel;
    }
    for (int i = currentStageIndex_; i < static_cast<int>(stageFuel_.size()); ++i) {
        fuel += std::max(0.0, stageFuel_[static_cast<size_t>(i)]);
    }
    return fuel;
}

double Rocket::currentThrust() const {
    const Stage* stage = currentStage();
    if (!stage || separationDelayRemaining_ > 0.0) {
        return 0.0;
    }
    const size_t index = static_cast<size_t>(currentStageIndex_);
    if (stageFuel_[index] <= 0.0 || stageElapsed_[index] >= stage->burnTime) {
        return 0.0;
    }
    return stage->thrust;
}

double Rocket::currentMassFlowRate() const {
    const Stage* stage = currentStage();
    if (!stage || separationDelayRemaining_ > 0.0) {
        return 0.0;
    }
    const size_t index = static_cast<size_t>(currentStageIndex_);
    if (stageFuel_[index] <= 0.0 || stageElapsed_[index] >= stage->burnTime) {
        return 0.0;
    }
    return stage->massFlowRate;
}

double Rocket::activeCdMultiplier() const {
    const Stage* stage = currentStage();
    return stage ? stage->cdMultiplier : 1.0;
}

double Rocket::activeAreaMultiplier() const {
    const Stage* stage = currentStage();
    return stage ? stage->areaMultiplier : 1.0;
}

int Rocket::currentStageIndex() const {
    return currentStageIndex_;
}

const Stage* Rocket::currentStage() const {
    if (currentStageIndex_ < 0 || currentStageIndex_ >= static_cast<int>(config_.stages.size())) {
        return nullptr;
    }
    return &config_.stages[static_cast<size_t>(currentStageIndex_)];
}

bool Rocket::hasActiveStage() const {
    return currentStage() != nullptr;
}

bool Rocket::isCoastingBetweenStages() const {
    return separationDelayRemaining_ > 0.0;
}

std::vector<StageEvent> Rocket::advanceAfterPhysicsStep(double dt,
                                                        double eventTime,
                                                        const Vec2& position,
                                                        double speed) {
    std::vector<StageEvent> events;
    if (currentStageIndex_ < 0 || currentStageIndex_ >= static_cast<int>(config_.stages.size())) {
        return events;
    }

    if (separationDelayRemaining_ > 0.0) {
        separationDelayRemaining_ = std::max(0.0, separationDelayRemaining_ - dt);
        return events;
    }

    const size_t index = static_cast<size_t>(currentStageIndex_);
    Stage& stage = config_.stages[index];

    const double remainingBurnTime = std::max(0.0, stage.burnTime - stageElapsed_[index]);
    const double burnDt = std::min(dt, remainingBurnTime);
    if (burnDt > 0.0 && stage.massFlowRate > 0.0 && stageFuel_[index] > 0.0) {
        const double burned = std::min(stageFuel_[index], stage.massFlowRate * burnDt);
        stageFuel_[index] = std::max(0.0, stageFuel_[index] - burned);
        stageElapsed_[index] += burnDt;
    } else {
        stageElapsed_[index] += dt;
    }

    const bool fuelEmpty = stageFuel_[index] <= 1.0e-6;
    const bool burnTimeDone = stageElapsed_[index] >= stage.burnTime - 1.0e-9;

    if (fuelEmpty || burnTimeDone) {
        StageEvent event;
        event.name = stage.name + " separation";
        event.stageIndex = currentStageIndex_;
        event.time = eventTime;
        event.x = position.x;
        event.altitude = position.y;
        event.velocity = speed;
        events.push_back(event);

        ++currentStageIndex_;
        if (currentStageIndex_ >= static_cast<int>(config_.stages.size())) {
            currentStageIndex_ = -1;
            separationDelayRemaining_ = 0.0;
        } else {
            separationDelayRemaining_ = stage.separationDelay;
        }
    }

    return events;
}

} // namespace rs
