#pragma once

#include <filesystem>
#include <vector>

#include "../physics/SimulationResult.h"

namespace rs {

class CSVWriter {
public:
    static bool write(const std::filesystem::path& path, const std::vector<TelemetrySample>& samples);
};

} // namespace rs
