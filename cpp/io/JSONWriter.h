#pragma once

#include <filesystem>

#include "../physics/SimulationConfig.h"
#include "../physics/SimulationResult.h"

namespace rs {

class JSONWriter {
public:
    static bool writeSummary(const std::filesystem::path& path,
                             const SimulationSummary& summary,
                             const SimulationConfig& config);
};

} // namespace rs
