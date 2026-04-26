#include "ConfigIO.h"

#include <cctype>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace rs {
namespace {

struct JsonValue {
    enum class Type { Null, Number, Boolean, String, Object, Array };

    Type type = Type::Null;
    double number = 0.0;
    bool boolean = false;
    std::string string;
    std::map<std::string, JsonValue> object;
    std::vector<JsonValue> array;
};

class JsonParser {
public:
    explicit JsonParser(std::string text) : text_(std::move(text)) {}

    JsonValue parse() {
        JsonValue value = parseValue();
        skipWhitespace();
        if (pos_ != text_.size()) {
            throw std::runtime_error("Unexpected trailing JSON text");
        }
        return value;
    }

private:
    std::string text_;
    size_t pos_ = 0;

    void skipWhitespace() {
        while (pos_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[pos_]))) {
            ++pos_;
        }
    }

    bool consume(char expected) {
        skipWhitespace();
        if (pos_ < text_.size() && text_[pos_] == expected) {
            ++pos_;
            return true;
        }
        return false;
    }

    char peek() {
        skipWhitespace();
        if (pos_ >= text_.size()) {
            throw std::runtime_error("Unexpected end of JSON");
        }
        return text_[pos_];
    }

    JsonValue parseValue() {
        const char c = peek();
        if (c == '{') {
            return parseObject();
        }
        if (c == '[') {
            return parseArray();
        }
        if (c == '"') {
            JsonValue value;
            value.type = JsonValue::Type::String;
            value.string = parseString();
            return value;
        }
        if (c == '-' || c == '+' || std::isdigit(static_cast<unsigned char>(c))) {
            return parseNumber();
        }
        if (text_.compare(pos_, 4, "true") == 0) {
            pos_ += 4;
            JsonValue value;
            value.type = JsonValue::Type::Boolean;
            value.boolean = true;
            return value;
        }
        if (text_.compare(pos_, 5, "false") == 0) {
            pos_ += 5;
            JsonValue value;
            value.type = JsonValue::Type::Boolean;
            value.boolean = false;
            return value;
        }
        if (text_.compare(pos_, 4, "null") == 0) {
            pos_ += 4;
            return {};
        }
        throw std::runtime_error("Invalid JSON value");
    }

    JsonValue parseObject() {
        if (!consume('{')) {
            throw std::runtime_error("Expected object");
        }

        JsonValue value;
        value.type = JsonValue::Type::Object;

        if (consume('}')) {
            return value;
        }

        while (true) {
            skipWhitespace();
            if (peek() != '"') {
                throw std::runtime_error("Expected object key");
            }
            std::string key = parseString();
            if (!consume(':')) {
                throw std::runtime_error("Expected ':' after key");
            }
            value.object[key] = parseValue();

            if (consume('}')) {
                break;
            }
            if (!consume(',')) {
                throw std::runtime_error("Expected ',' in object");
            }
        }
        return value;
    }

    JsonValue parseArray() {
        if (!consume('[')) {
            throw std::runtime_error("Expected array");
        }

        JsonValue value;
        value.type = JsonValue::Type::Array;

        if (consume(']')) {
            return value;
        }

        while (true) {
            value.array.push_back(parseValue());
            if (consume(']')) {
                break;
            }
            if (!consume(',')) {
                throw std::runtime_error("Expected ',' in array");
            }
        }
        return value;
    }

    std::string parseString() {
        if (!consume('"')) {
            throw std::runtime_error("Expected string");
        }

        std::string result;
        while (pos_ < text_.size()) {
            const char c = text_[pos_++];
            if (c == '"') {
                return result;
            }
            if (c == '\\') {
                if (pos_ >= text_.size()) {
                    throw std::runtime_error("Invalid escape");
                }
                const char escaped = text_[pos_++];
                switch (escaped) {
                    case '"':
                    case '\\':
                    case '/':
                        result.push_back(escaped);
                        break;
                    case 'b':
                        result.push_back('\b');
                        break;
                    case 'f':
                        result.push_back('\f');
                        break;
                    case 'n':
                        result.push_back('\n');
                        break;
                    case 'r':
                        result.push_back('\r');
                        break;
                    case 't':
                        result.push_back('\t');
                        break;
                    default:
                        result.push_back(escaped);
                        break;
                }
            } else {
                result.push_back(c);
            }
        }
        throw std::runtime_error("Unterminated string");
    }

    JsonValue parseNumber() {
        skipWhitespace();
        const size_t start = pos_;
        if (pos_ < text_.size() && (text_[pos_] == '+' || text_[pos_] == '-')) {
            ++pos_;
        }
        while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) {
            ++pos_;
        }
        if (pos_ < text_.size() && text_[pos_] == '.') {
            ++pos_;
            while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) {
                ++pos_;
            }
        }
        if (pos_ < text_.size() && (text_[pos_] == 'e' || text_[pos_] == 'E')) {
            ++pos_;
            if (pos_ < text_.size() && (text_[pos_] == '+' || text_[pos_] == '-')) {
                ++pos_;
            }
            while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) {
                ++pos_;
            }
        }

        JsonValue value;
        value.type = JsonValue::Type::Number;
        value.number = std::stod(text_.substr(start, pos_ - start));
        return value;
    }
};

std::string indent(int spaces) {
    return std::string(static_cast<size_t>(spaces), ' ');
}

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

const JsonValue* objectValue(const JsonValue& object, const std::string& key) {
    if (object.type != JsonValue::Type::Object) {
        return nullptr;
    }
    const auto it = object.object.find(key);
    if (it == object.object.end()) {
        return nullptr;
    }
    return &it->second;
}

double numberOr(const JsonValue& object, const std::string& key, double fallback) {
    const JsonValue* value = objectValue(object, key);
    if (!value || value->type != JsonValue::Type::Number) {
        return fallback;
    }
    return value->number;
}

bool boolOr(const JsonValue& object, const std::string& key, bool fallback) {
    const JsonValue* value = objectValue(object, key);
    if (!value || value->type != JsonValue::Type::Boolean) {
        return fallback;
    }
    return value->boolean;
}

std::string stringOr(const JsonValue& object, const std::string& key, const std::string& fallback) {
    const JsonValue* value = objectValue(object, key);
    if (!value || value->type != JsonValue::Type::String) {
        return fallback;
    }
    return value->string;
}

Stage stageFromJson(const JsonValue& value, const Stage& fallback) {
    if (value.type != JsonValue::Type::Object) {
        return fallback;
    }

    Stage stage = fallback;
    stage.name = stringOr(value, "name", stage.name);
    stage.dryMass = numberOr(value, "dryMass", stage.dryMass);
    stage.fuelMass = numberOr(value, "fuelMass", stage.fuelMass);
    stage.thrust = numberOr(value, "thrust", stage.thrust);
    stage.massFlowRate = numberOr(value, "massFlowRate", stage.massFlowRate);
    stage.burnTime = numberOr(value, "burnTime", stage.burnTime);
    stage.separationDelay = numberOr(value, "separationDelay", stage.separationDelay);
    stage.cdMultiplier = numberOr(value, "cdMultiplier", stage.cdMultiplier);
    stage.areaMultiplier = numberOr(value, "areaMultiplier", stage.areaMultiplier);
    return stage;
}

SimulationConfig configFromJson(const JsonValue& root, const SimulationConfig& fallback) {
    if (root.type != JsonValue::Type::Object) {
        return fallback;
    }

    SimulationConfig config = fallback;
    config.presetName = stringOr(root, "presetName", config.presetName);
    config.dt = numberOr(root, "dt", config.dt);
    config.maxTime = numberOr(root, "maxTime", config.maxTime);
    config.timeScale = numberOr(root, "timeScale", config.timeScale);
    config.initialDownrange = numberOr(root, "initialDownrange", config.initialDownrange);
    config.initialAltitude = numberOr(root, "initialAltitude", config.initialAltitude);
    config.initialVelocity = numberOr(root, "initialVelocity", config.initialVelocity);
    config.initialAngleDeg = numberOr(root, "initialAngleDeg", config.initialAngleDeg);
    config.payloadMass = numberOr(root, "payloadMass", config.payloadMass);
    config.baseCd = numberOr(root, "baseCd", config.baseCd);
    config.baseArea = numberOr(root, "baseArea", config.baseArea);
    config.atmosphereEnabled = boolOr(root, "atmosphereEnabled", config.atmosphereEnabled);
    config.pitchStartTime = numberOr(root, "pitchStartTime", config.pitchStartTime);
    config.pitchRateDegPerSec = numberOr(root, "pitchRateDegPerSec", config.pitchRateDegPerSec);
    config.minPitchDeg = numberOr(root, "minPitchDeg", config.minPitchDeg);

    const std::string integrator = stringOr(root, "integrator", integratorName(config.integrator));
    config.integrator = integrator == "Euler" ? IntegratorType::Euler : IntegratorType::RK4;

    const JsonValue* stages = objectValue(root, "stages");
    if (stages && stages->type == JsonValue::Type::Array) {
        config.stages.clear();
        for (size_t i = 0; i < stages->array.size(); ++i) {
            Stage fallbackStage;
            fallbackStage.name = "Stage " + std::to_string(i + 1);
            config.stages.push_back(stageFromJson(stages->array[i], fallbackStage));
        }
    }

    return config;
}

std::string stageToJson(const Stage& stage, int spaces) {
    std::ostringstream out;
    out << indent(spaces) << "{\n";
    out << indent(spaces + 2) << "\"name\": \"" << escapeJson(stage.name) << "\",\n";
    out << indent(spaces + 2) << "\"dryMass\": " << stage.dryMass << ",\n";
    out << indent(spaces + 2) << "\"fuelMass\": " << stage.fuelMass << ",\n";
    out << indent(spaces + 2) << "\"thrust\": " << stage.thrust << ",\n";
    out << indent(spaces + 2) << "\"massFlowRate\": " << stage.massFlowRate << ",\n";
    out << indent(spaces + 2) << "\"burnTime\": " << stage.burnTime << ",\n";
    out << indent(spaces + 2) << "\"separationDelay\": " << stage.separationDelay << ",\n";
    out << indent(spaces + 2) << "\"cdMultiplier\": " << stage.cdMultiplier << ",\n";
    out << indent(spaces + 2) << "\"areaMultiplier\": " << stage.areaMultiplier << "\n";
    out << indent(spaces) << "}";
    return out.str();
}

} // namespace

SimulationConfig ConfigIO::defaultConfig() {
    SimulationConfig config;
    config.presetName = "Heavy Artemis-like Rocket";
    config.dt = 0.02;
    config.maxTime = 800.0;
    config.timeScale = 10.0;
    config.initialDownrange = 0.0;
    config.initialAltitude = 0.0;
    config.initialVelocity = 0.0;
    config.initialAngleDeg = 90.0;
    config.payloadMass = 15000.0;
    config.baseCd = 0.35;
    config.baseArea = 80.0;
    config.atmosphereEnabled = true;
    config.pitchStartTime = 8.0;
    config.pitchRateDegPerSec = 0.34;
    config.minPitchDeg = 6.0;
    config.integrator = IntegratorType::RK4;

    Stage stage1;
    stage1.name = "Core Stage";
    stage1.dryMass = 85000.0;
    stage1.fuelMass = 400000.0;
    stage1.thrust = 7600000.0;
    stage1.massFlowRate = 2500.0;
    stage1.burnTime = 160.0;
    stage1.separationDelay = 2.0;
    stage1.cdMultiplier = 1.0;
    stage1.areaMultiplier = 1.0;

    Stage stage2;
    stage2.name = "Upper Stage";
    stage2.dryMass = 20000.0;
    stage2.fuelMass = 110000.0;
    stage2.thrust = 1000000.0;
    stage2.massFlowRate = 350.0;
    stage2.burnTime = 300.0;
    stage2.separationDelay = 1.0;
    stage2.cdMultiplier = 0.75;
    stage2.areaMultiplier = 0.75;

    config.stages = {stage1, stage2};
    return config;
}

std::vector<SimulationConfig> ConfigIO::presets() {
    std::vector<SimulationConfig> result;

    SimulationConfig small = defaultConfig();
    small.presetName = "Small Educational Rocket";
    small.dt = 0.02;
    small.maxTime = 260.0;
    small.timeScale = 5.0;
    small.payloadMass = 200.0;
    small.baseCd = 0.42;
    small.baseArea = 1.8;
    small.pitchStartTime = 5.0;
    small.pitchRateDegPerSec = 0.7;
    small.minPitchDeg = 12.0;
    small.stages.resize(1);
    small.stages[0].name = "Educational Booster";
    small.stages[0].dryMass = 800.0;
    small.stages[0].fuelMass = 4200.0;
    small.stages[0].thrust = 140000.0;
    small.stages[0].massFlowRate = 55.0;
    small.stages[0].burnTime = 75.0;
    small.stages[0].separationDelay = 0.0;
    small.stages[0].cdMultiplier = 1.0;
    small.stages[0].areaMultiplier = 1.0;
    result.push_back(small);

    result.push_back(defaultConfig());

    SimulationConfig highDrag = defaultConfig();
    highDrag.presetName = "High Drag Test";
    highDrag.baseCd = 1.2;
    highDrag.baseArea = 150.0;
    highDrag.timeScale = 8.0;
    highDrag.pitchRateDegPerSec = 0.2;
    result.push_back(highDrag);

    SimulationConfig noAtmosphere = defaultConfig();
    noAtmosphere.presetName = "No Atmosphere Test";
    noAtmosphere.atmosphereEnabled = false;
    noAtmosphere.timeScale = 20.0;
    result.push_back(noAtmosphere);

    SimulationConfig eulerTest = defaultConfig();
    eulerTest.presetName = "Euler vs RK4 Test";
    eulerTest.integrator = IntegratorType::Euler;
    eulerTest.dt = 0.08;
    eulerTest.maxTime = 500.0;
    eulerTest.timeScale = 10.0;
    result.push_back(eulerTest);

    return result;
}

bool ConfigIO::loadConfig(const std::filesystem::path& path, SimulationConfig& config) {
    std::ifstream in(path);
    if (!in) {
        config = defaultConfig();
        return false;
    }

    std::ostringstream buffer;
    buffer << in.rdbuf();

    try {
        JsonParser parser(buffer.str());
        const JsonValue root = parser.parse();

        const JsonValue* configObject = objectValue(root, "simulationConfig");
        if (!configObject) {
            configObject = &root;
        }

        config = configFromJson(*configObject, defaultConfig());
        return true;
    } catch (const std::exception&) {
        config = defaultConfig();
        return false;
    }
}

bool ConfigIO::saveConfig(const std::filesystem::path& path,
                          const SimulationConfig& config,
                          bool includePresets) {
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream out(path);
    if (!out) {
        return false;
    }

    out << "{\n";
    out << "  \"simulationConfig\": " << configToJsonObject(config, 2);
    if (includePresets) {
        out << ",\n  \"presets\": [\n";
        const std::vector<SimulationConfig> allPresets = presets();
        for (size_t i = 0; i < allPresets.size(); ++i) {
            out << configToJsonObject(allPresets[i], 4);
            if (i + 1 < allPresets.size()) {
                out << ',';
            }
            out << '\n';
        }
        out << "  ]\n";
    } else {
        out << '\n';
    }
    out << "}\n";
    return true;
}

bool ConfigIO::ensureDefaultConfig(const std::filesystem::path& path) {
    if (std::filesystem::exists(path)) {
        return true;
    }
    return saveConfig(path, defaultConfig(), true);
}

std::string ConfigIO::configToJsonObject(const SimulationConfig& config, int indentSpaces) {
    std::ostringstream out;
    out << std::setprecision(12);
    out << "{\n";
    out << indent(indentSpaces + 2) << "\"presetName\": \"" << escapeJson(config.presetName) << "\",\n";
    out << indent(indentSpaces + 2) << "\"dt\": " << config.dt << ",\n";
    out << indent(indentSpaces + 2) << "\"maxTime\": " << config.maxTime << ",\n";
    out << indent(indentSpaces + 2) << "\"timeScale\": " << config.timeScale << ",\n";
    out << indent(indentSpaces + 2) << "\"initialDownrange\": " << config.initialDownrange << ",\n";
    out << indent(indentSpaces + 2) << "\"initialAltitude\": " << config.initialAltitude << ",\n";
    out << indent(indentSpaces + 2) << "\"initialVelocity\": " << config.initialVelocity << ",\n";
    out << indent(indentSpaces + 2) << "\"initialAngleDeg\": " << config.initialAngleDeg << ",\n";
    out << indent(indentSpaces + 2) << "\"payloadMass\": " << config.payloadMass << ",\n";
    out << indent(indentSpaces + 2) << "\"baseCd\": " << config.baseCd << ",\n";
    out << indent(indentSpaces + 2) << "\"baseArea\": " << config.baseArea << ",\n";
    out << indent(indentSpaces + 2) << "\"atmosphereEnabled\": " << (config.atmosphereEnabled ? "true" : "false") << ",\n";
    out << indent(indentSpaces + 2) << "\"pitchStartTime\": " << config.pitchStartTime << ",\n";
    out << indent(indentSpaces + 2) << "\"pitchRateDegPerSec\": " << config.pitchRateDegPerSec << ",\n";
    out << indent(indentSpaces + 2) << "\"minPitchDeg\": " << config.minPitchDeg << ",\n";
    out << indent(indentSpaces + 2) << "\"integrator\": \"" << integratorName(config.integrator) << "\",\n";
    out << indent(indentSpaces + 2) << "\"stages\": [\n";

    for (size_t i = 0; i < config.stages.size(); ++i) {
        out << stageToJson(config.stages[i], indentSpaces + 4);
        if (i + 1 < config.stages.size()) {
            out << ',';
        }
        out << '\n';
    }

    out << indent(indentSpaces + 2) << "]\n";
    out << indent(indentSpaces) << "}";
    return out.str();
}

} // namespace rs
