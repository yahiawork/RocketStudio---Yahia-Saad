#pragma once

#include <filesystem>
#include <string>

#include "Renderer2D.h"
#include "Renderer3D.h"
#include "../physics/PhysicsEngine.h"

namespace rs {

class Gui {
public:
    Gui();
    void render(PhysicsEngine& engine, Renderer2D& renderer2D, Renderer3D& renderer3D, double fps);

private:
    std::filesystem::path configPath_;
    std::filesystem::path outputPath_;
    std::filesystem::path summaryPath_;
    std::string message_;
    bool showCreditsLicense_ = false;

    void drawTopBar(const PhysicsEngine& engine, double fps, const ImVec2& pos, const ImVec2& size);
    void drawControls(PhysicsEngine& engine, const ImVec2& pos, const ImVec2& size);
    void drawCenter(PhysicsEngine& engine, Renderer2D& renderer2D, Renderer3D& renderer3D, const ImVec2& pos, const ImVec2& size);
    void drawCharts(const PhysicsEngine& engine, const ImVec2& pos, const ImVec2& size);
    void drawTelemetry(const PhysicsEngine& engine, const ImVec2& pos, const ImVec2& size);
    void drawPhysicsExplanation();
    void drawCreditsLicenseWindow();
    void exportData(const PhysicsEngine& engine);
};

} // namespace rs
