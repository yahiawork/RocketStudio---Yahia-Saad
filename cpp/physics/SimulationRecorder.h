#pragma once

#include <vector>

#include "SimulationConfig.h"
#include "SimulationResult.h"

namespace rs {

class SimulationRecorder {
public:
    void clear();
    void recordSample(const TelemetrySample& sample, bool escapeVelocityReachedNow);
    void recordStageEvent(const StageEvent& event);

    const std::vector<TelemetrySample>& samples() const;
    const std::vector<StageEvent>& stageEvents() const;
    const TelemetrySample* latest() const;
    SimulationSummary summary() const;

private:
    std::vector<TelemetrySample> samples_;
    std::vector<StageEvent> stageEvents_;
    SimulationSummary summary_;
};

} // namespace rs
