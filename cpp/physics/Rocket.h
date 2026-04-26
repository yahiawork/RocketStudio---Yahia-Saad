#pragma once

#include <vector>

#include "SimulationConfig.h"
#include "SimulationResult.h"
#include "Vec2.h"

namespace rs {

class Rocket {
public:
    Rocket();
    explicit Rocket(const SimulationConfig& config);

    void reset(const SimulationConfig& config);

    double totalMass() const;
    double fuelRemaining() const;
    double currentThrust() const;
    double currentMassFlowRate() const;
    double activeCdMultiplier() const;
    double activeAreaMultiplier() const;
    int currentStageIndex() const;
    const Stage* currentStage() const;
    bool hasActiveStage() const;
    bool isCoastingBetweenStages() const;

    std::vector<StageEvent> advanceAfterPhysicsStep(double dt, double eventTime, const Vec2& position, double speed);

private:
    SimulationConfig config_;
    std::vector<double> stageFuel_;
    std::vector<double> stageElapsed_;
    int currentStageIndex_ = 0;
    double separationDelayRemaining_ = 0.0;

    void normalizeConfig();
};

} // namespace rs
