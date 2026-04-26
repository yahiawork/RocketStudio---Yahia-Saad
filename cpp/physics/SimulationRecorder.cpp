#include "SimulationRecorder.h"

#include <algorithm>

namespace rs {

void SimulationRecorder::clear() {
    samples_.clear();
    stageEvents_.clear();
    summary_ = SimulationSummary{};
}

void SimulationRecorder::recordSample(const TelemetrySample& sample, bool escapeVelocityReachedNow) {
    samples_.push_back(sample);

    if (samples_.size() == 1 || sample.altitude > summary_.maxAltitude) {
        summary_.maxAltitude = sample.altitude;
        summary_.maxAltitudeTime = sample.time;
    }
    if (samples_.size() == 1 || sample.speed > summary_.maxVelocity) {
        summary_.maxVelocity = sample.speed;
        summary_.maxVelocityTime = sample.time;
    }
    if (samples_.size() == 1 || sample.acceleration > summary_.maxAcceleration) {
        summary_.maxAcceleration = sample.acceleration;
    }
    if (samples_.size() == 1 || sample.dynamicPressure > summary_.maxQ) {
        summary_.maxQ = sample.dynamicPressure;
        summary_.maxQTime = sample.time;
        summary_.maxQAltitude = sample.altitude;
        summary_.maxQVelocity = sample.speed;
    }

    summary_.finalTime = sample.time;
    summary_.finalAltitude = sample.altitude;
    summary_.finalVelocity = sample.speed;
    summary_.escapeVelocityReached = summary_.escapeVelocityReached || escapeVelocityReachedNow;
}

void SimulationRecorder::recordStageEvent(const StageEvent& event) {
    stageEvents_.push_back(event);
    summary_.stageEvents = stageEvents_;
}

const std::vector<TelemetrySample>& SimulationRecorder::samples() const {
    return samples_;
}

const std::vector<StageEvent>& SimulationRecorder::stageEvents() const {
    return stageEvents_;
}

const TelemetrySample* SimulationRecorder::latest() const {
    if (samples_.empty()) {
        return nullptr;
    }
    return &samples_.back();
}

SimulationSummary SimulationRecorder::summary() const {
    SimulationSummary result = summary_;
    result.stageEvents = stageEvents_;
    return result;
}

} // namespace rs
