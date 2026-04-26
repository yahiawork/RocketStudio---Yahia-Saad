#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "../physics/SimulationConfig.h"

namespace rs {

class ConfigIO {
public:
    static SimulationConfig defaultConfig();
    static std::vector<SimulationConfig> presets();

    static bool loadConfig(const std::filesystem::path& path, SimulationConfig& config);
    static bool saveConfig(const std::filesystem::path& path, const SimulationConfig& config, bool includePresets = true);
    static bool ensureDefaultConfig(const std::filesystem::path& path);

    static std::string configToJsonObject(const SimulationConfig& config, int indentSpaces = 2);
};

} // namespace rs
