#include "Logger.h"

#include <iostream>

namespace rs {

void Logger::info(const std::string& message) {
    std::cout << "[INFO] " << message << '\n';
}

void Logger::warn(const std::string& message) {
    std::cout << "[WARN] " << message << '\n';
}

void Logger::error(const std::string& message) {
    std::cerr << "[ERROR] " << message << '\n';
}

} // namespace rs
