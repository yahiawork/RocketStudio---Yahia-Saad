#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "app/App.h"
#include "io/CSVWriter.h"
#include "io/ConfigIO.h"
#include "io/JSONWriter.h"
#include "physics/Environment.h"
#include "physics/PhysicsEngine.h"

namespace {

bool hasArg(const std::vector<std::string>& args, const std::string& name) {
    return std::find(args.begin(), args.end(), name) != args.end();
}

std::string argValue(const std::vector<std::string>& args, const std::string& name, const std::string& fallback) {
    for (size_t i = 0; i + 1 < args.size(); ++i) {
        if (args[i] == name) {
            return args[i + 1];
        }
    }
    return fallback;
}

void printUsage() {
    std::cout << "RocketStudio usage:\n"
              << "  RocketStudio.exe\n"
              << "  RocketStudio.exe --headless --config ../data/default_config.json --output ../data/output.csv --summary ../data/summary.json\n"
              << "  RocketStudio.exe --test\n";
}

bool samplesAreFiniteAndNonNegative(const std::vector<rs::TelemetrySample>& samples) {
    if (samples.empty()) {
        return false;
    }
    for (const rs::TelemetrySample& sample : samples) {
        const bool finite = std::isfinite(sample.time) &&
                            std::isfinite(sample.x) &&
                            std::isfinite(sample.altitude) &&
                            std::isfinite(sample.vx) &&
                            std::isfinite(sample.vy) &&
                            std::isfinite(sample.speed) &&
                            std::isfinite(sample.mass) &&
                            std::isfinite(sample.fuelRemaining);
        if (!finite || sample.mass <= 0.0 || sample.fuelRemaining < -1.0e-6) {
            return false;
        }
    }
    return true;
}

int runTests() {
    int failures = 0;
    auto check = [&](const std::string& name, bool condition) {
        std::cout << (condition ? "PASS " : "FAIL ") << name << '\n';
        if (!condition) {
            ++failures;
        }
    };

    check("gravity decreases with altitude",
          rs::Environment::gravityAtAltitude(0.0) > rs::Environment::gravityAtAltitude(100000.0));
    check("density decreases with altitude",
          rs::Environment::densityAtAltitude(0.0) > rs::Environment::densityAtAltitude(50000.0));
    check("drag is zero when velocity is zero",
          rs::Environment::dragMagnitude(1.225, 0.0, 0.5, 10.0) == 0.0);

    rs::SimulationConfig euler = rs::ConfigIO::defaultConfig();
    euler.integrator = rs::IntegratorType::Euler;
    euler.dt = 0.05;
    euler.maxTime = 40.0;
    rs::PhysicsEngine eulerEngine(euler);
    eulerEngine.start();
    eulerEngine.runToEnd();
    check("Euler runs without NaN", samplesAreFiniteAndNonNegative(eulerEngine.recorder().samples()));

    rs::SimulationConfig rk4 = rs::ConfigIO::defaultConfig();
    rk4.integrator = rs::IntegratorType::RK4;
    rk4.dt = 0.05;
    rk4.maxTime = 40.0;
    rs::PhysicsEngine rk4Engine(rk4);
    rk4Engine.start();
    rk4Engine.runToEnd();
    check("RK4 runs without NaN", samplesAreFiniteAndNonNegative(rk4Engine.recorder().samples()));
    check("mass never becomes negative", samplesAreFiniteAndNonNegative(rk4Engine.recorder().samples()));
    check("fuel never becomes negative", samplesAreFiniteAndNonNegative(rk4Engine.recorder().samples()));

    if (failures == 0) {
        std::cout << "All RocketStudio validation checks passed.\n";
    } else {
        std::cout << failures << " validation check(s) failed.\n";
    }
    return failures == 0 ? 0 : 1;
}

int runHeadless(const std::vector<std::string>& args) {
    const std::filesystem::path configPath = argValue(args, "--config", "../data/default_config.json");
    const std::filesystem::path outputPath = argValue(args, "--output", "../data/output.csv");
    const std::filesystem::path summaryPath = argValue(args, "--summary", "../data/summary.json");

    rs::ConfigIO::ensureDefaultConfig(configPath);

    rs::SimulationConfig config;
    if (!rs::ConfigIO::loadConfig(configPath, config)) {
        std::cout << "Config missing or invalid. Using built-in default configuration.\n";
    }

    rs::PhysicsEngine engine(config);
    engine.start();
    engine.runToEnd();

    const bool csvOk = rs::CSVWriter::write(outputPath, engine.recorder().samples());
    const bool jsonOk = rs::JSONWriter::writeSummary(summaryPath, engine.recorder().summary(), engine.config());

    std::cout << "Headless simulation finished.\n"
              << "  Samples: " << engine.recorder().samples().size() << '\n'
              << "  Final time: " << engine.recorder().summary().finalTime << " s\n"
              << "  Max altitude: " << engine.recorder().summary().maxAltitude << " m\n"
              << "  CSV: " << outputPath.string() << (csvOk ? " [ok]" : " [failed]") << '\n'
              << "  JSON: " << summaryPath.string() << (jsonOk ? " [ok]" : " [failed]") << '\n';

    return csvOk && jsonOk ? 0 : 1;
}

} // namespace

int main(int argc, char** argv) {
    std::vector<std::string> args;
    args.reserve(static_cast<size_t>(argc));
    for (int i = 0; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    if (hasArg(args, "--help") || hasArg(args, "-h")) {
        printUsage();
        return 0;
    }

    if (hasArg(args, "--test")) {
        return runTests();
    }

    if (hasArg(args, "--headless")) {
        return runHeadless(args);
    }

    rs::App app;
    return app.run();
}
