#include "JSONWriter.h"

#include <fstream>
#include <iomanip>
#include <sstream>

#include "ConfigIO.h"

namespace rs {
namespace {

std::string escapeJson(const std::string& input) {
    std::ostringstream out;
    for (const char c : input) {
        switch (c) {
            case '"':
                out << "\\\"";
                break;
            case '\\':
                out << "\\\\";
                break;
            case '\n':
                out << "\\n";
                break;
            case '\r':
                out << "\\r";
                break;
            case '\t':
                out << "\\t";
                break;
            default:
                out << c;
                break;
        }
    }
    return out.str();
}

} // namespace

bool JSONWriter::writeSummary(const std::filesystem::path& path,
                              const SimulationSummary& summary,
                              const SimulationConfig& config) {
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream out(path);
    if (!out) {
        return false;
    }

    out << std::setprecision(12);
    out << "{\n";
    out << "  \"maxAltitude\": " << summary.maxAltitude << ",\n";
    out << "  \"maxAltitudeTime\": " << summary.maxAltitudeTime << ",\n";
    out << "  \"maxVelocity\": " << summary.maxVelocity << ",\n";
    out << "  \"maxVelocityTime\": " << summary.maxVelocityTime << ",\n";
    out << "  \"maxAcceleration\": " << summary.maxAcceleration << ",\n";
    out << "  \"maxQ\": " << summary.maxQ << ",\n";
    out << "  \"maxQTime\": " << summary.maxQTime << ",\n";
    out << "  \"maxQAltitude\": " << summary.maxQAltitude << ",\n";
    out << "  \"maxQVelocity\": " << summary.maxQVelocity << ",\n";
    out << "  \"finalTime\": " << summary.finalTime << ",\n";
    out << "  \"finalAltitude\": " << summary.finalAltitude << ",\n";
    out << "  \"finalVelocity\": " << summary.finalVelocity << ",\n";
    out << "  \"escapeVelocityReached\": " << (summary.escapeVelocityReached ? "true" : "false") << ",\n";
    out << "  \"stageEvents\": [\n";

    for (size_t i = 0; i < summary.stageEvents.size(); ++i) {
        const StageEvent& event = summary.stageEvents[i];
        out << "    {\n";
        out << "      \"name\": \"" << escapeJson(event.name) << "\",\n";
        out << "      \"stageIndex\": " << event.stageIndex << ",\n";
        out << "      \"time\": " << event.time << ",\n";
        out << "      \"x\": " << event.x << ",\n";
        out << "      \"altitude\": " << event.altitude << ",\n";
        out << "      \"velocity\": " << event.velocity << "\n";
        out << "    }";
        if (i + 1 < summary.stageEvents.size()) {
            out << ',';
        }
        out << '\n';
    }

    out << "  ],\n";
    out << "  \"simulationConfig\": " << ConfigIO::configToJsonObject(config, 2) << "\n";
    out << "}\n";
    return true;
}

} // namespace rs
