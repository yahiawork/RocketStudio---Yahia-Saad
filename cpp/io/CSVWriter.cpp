#include "CSVWriter.h"

#include <fstream>
#include <iomanip>

namespace rs {

bool CSVWriter::write(const std::filesystem::path& path, const std::vector<TelemetrySample>& samples) {
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream out(path);
    if (!out) {
        return false;
    }

    out << "time,x,altitude,vx,vy,speed,ax,ay,acceleration,mass,fuel_remaining,thrust,drag,gravity,density,dynamic_pressure,current_stage\n";
    out << std::setprecision(12);

    for (const TelemetrySample& s : samples) {
        out << s.time << ','
            << s.x << ','
            << s.altitude << ','
            << s.vx << ','
            << s.vy << ','
            << s.speed << ','
            << s.ax << ','
            << s.ay << ','
            << s.acceleration << ','
            << s.mass << ','
            << s.fuelRemaining << ','
            << s.thrust << ','
            << s.drag << ','
            << s.gravity << ','
            << s.density << ','
            << s.dynamicPressure << ','
            << s.currentStage << '\n';
    }

    return true;
}

} // namespace rs
