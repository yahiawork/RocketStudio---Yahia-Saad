#include "Gui.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include <imgui.h>
#include <implot.h>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <shellapi.h>
#endif

#include "../io/CSVWriter.h"
#include "../io/ConfigIO.h"
#include "../io/JSONWriter.h"
#include "../physics/Environment.h"
#include "../utils/MathUtils.h"

namespace rs {
namespace {

constexpr ImGuiWindowFlags panelFlags =
    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

constexpr const char* kLicenseText = R"(First Stand Studio Proprietary License v1.0

Copyright (c) 2026 Yahia Saad / First Stand Studio.
All rights reserved.

1. Scope and Ownership

This repository, including without limitation its source code, design, layout,
copy, media, branding, visual identity, documentation, and derivative material
prepared by or for the copyright holder, is proprietary and protected by
applicable copyright and intellectual property laws, as well as international
treaties, including the Berne Convention for the Protection of Literary and
Artistic Works and the WIPO Copyright Treaty, where applicable.

All right, title, and interest in and to the repository remain exclusively with
Yahia Saad / First Stand Studio, except for third-party materials that are
expressly identified as being subject to their own licenses.

2. No Open-Source Grant

This repository is source-available only for limited review under the terms of
this license. It is not open-source software. No permission is granted except
as expressly stated in this license. Any right not expressly granted is
reserved.

3. Limited Permission

Subject to full compliance with this license, you may:

- View the repository and its contents for personal review.
- Retain one private copy solely for backup or private evaluation.
- Quote small, non-substantial excerpts only when required for commentary,
  review, or reporting that is otherwise permitted by applicable law.

No broader right to use, reproduce, modify, publish, distribute, sublicense, or
commercialize any part of the repository is granted.

4. Prohibited Conduct

Without prior express written permission from Yahia Saad / First Stand Studio,
you may not, directly or indirectly:

- Copy, reproduce, republish, upload, post, transmit, or distribute the
  repository or any substantial part of it.
- Modify, adapt, translate, reverse engineer into a competing implementation,
  or create derivative works based on the repository.
- Sell, license, lease, rent, sublicense, monetize, or otherwise exploit the
  repository for commercial or business purposes.
- Re-upload, mirror, or redistribute the repository in whole or in substantial
  part on GitHub, GitLab, websites, archives, packages, templates, courses,
  marketplaces, or similar services.
- Use the repository or any substantial portion of it in client work, freelance
  work, internal business systems, commercial products, SaaS, or paid services.
- Remove, alter, obscure, or bypass any copyright notice, ownership notice,
  branding, attribution, watermark, or license notice.
- Use the name "First Stand Studio", "Yahia Saad", related marks, or confusingly
  similar names, marks, or branding in a way that implies authorship,
  endorsement, association, or origin.
- Use the repository to train, fine-tune, evaluate, or improve machine
  learning, generative AI, code generation, or dataset systems, or include the
  repository in any dataset, benchmark, corpus, or model-training pipeline.
- Use the repository to build, market, or support a competing project, service,
  framework, template, or product derived from this work.

5. Forks and Platform Copies

Any fork, mirror, cache, or platform-generated copy permitted by the mechanics
of a hosting service remains subject to this license. Technical ability to fork
or clone does not create permission to use, publish, distribute, or exploit the
repository beyond the limited rights expressly granted here.

6. Third-Party Components

Third-party dependencies, packages, fonts, images, libraries, or other
materials included in or referenced by the repository may be subject to their
own license terms. This license applies only to the rights owned by Yahia Saad /
First Stand Studio and does not replace or override third-party license
obligations.

7. Attribution and Notices

If the copyright holder grants written permission for any exceptional use, all
copyright, ownership, authorship, and license notices must be preserved exactly
as provided unless the copyright holder expressly authorizes otherwise in
writing.

8. Enforcement and Remedies

Any use of the repository outside the permissions granted by this license is
unauthorized and constitutes infringement to the maximum extent permitted by
applicable law.

The copyright holder reserves all legal and equitable remedies, including
without limitation:

- Requests for immediate removal or takedown.
- Revocation of any previously granted permission.
- Cease-and-desist demands.
- Claims for injunctive relief.
- Claims for damages, profits, costs, and attorneys' fees where recoverable.
- Submission of copyright infringement notices, including DMCA notices on
  supported platforms where applicable.

9. Disclaimer

The repository is provided "AS IS", without warranty of any kind, express or
implied, including without limitation warranties of merchantability, fitness
for a particular purpose, title, non-infringement, availability, or accuracy.

10. No Waiver

Failure to enforce any provision of this license does not constitute a waiver of
that provision or any other right.

11. Severability

If any provision of this license is held unenforceable, the remaining
provisions shall remain in effect to the fullest extent permitted by applicable
law.

12. Contact for Written Permission

For licensing requests, commercial authorization, or written permission, contact
the copyright holder directly:

Yahia Saad
yahiasaad.work@gmail.com

13. Non-Waivable Rights

Nothing in this license is intended to limit any non-waivable rights,
limitations, or exceptions that may apply under applicable law.)";

bool dragDouble(const char* label, double& value, double speed, double minValue, double maxValue, const char* format) {
    double minCopy = minValue;
    double maxCopy = maxValue;
    const bool changed = ImGui::DragScalar(label, ImGuiDataType_Double, &value, static_cast<float>(speed), &minCopy, &maxCopy, format);
    if (changed) {
        value = math::clamp(value, minValue, maxValue);
    }
    return changed;
}

void valueRow(const char* label, const char* format, double value) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::TextUnformatted(label);
    ImGui::TableNextColumn();
    ImGui::Text(format, value);
}

void valueRowText(const char* label, const char* value) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::TextUnformatted(label);
    ImGui::TableNextColumn();
    ImGui::TextUnformatted(value);
}

void openUrl(const char* url) {
#ifdef _WIN32
    ShellExecuteA(nullptr, "open", url, nullptr, nullptr, SW_SHOWNORMAL);
#elif defined(__APPLE__)
    std::string command = "open \"";
    command += url;
    command += "\"";
    std::system(command.c_str());
#else
    std::string command = "xdg-open \"";
    command += url;
    command += "\"";
    std::system(command.c_str());
#endif
}

void clickableLink(const char* label, const char* url) {
    ImGui::TextColored(ImVec4(0.33f, 0.65f, 0.96f, 1.0f), "%s", label);
    if (ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        ImGui::SetTooltip("Open %s", url);
        const ImVec2 min = ImGui::GetItemRectMin();
        const ImVec2 max = ImGui::GetItemRectMax();
        ImGui::GetWindowDrawList()->AddLine(ImVec2(min.x, max.y), max, ImGui::GetColorU32(ImVec4(0.33f, 0.65f, 0.96f, 1.0f)), 1.0f);
    }
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        openUrl(url);
    }
}

template <typename Getter>
void plotMetric(const char* title,
                const std::vector<TelemetrySample>& samples,
                Getter getter,
                const ImVec4& color) {
    if (samples.empty()) {
        ImGui::TextDisabled("No samples yet");
        return;
    }

    const size_t stride = std::max<size_t>(1, samples.size() / 1800);
    std::vector<double> x;
    std::vector<double> y;
    x.reserve(samples.size() / stride + 1);
    y.reserve(samples.size() / stride + 1);
    for (size_t i = 0; i < samples.size(); i += stride) {
        x.push_back(samples[i].time);
        y.push_back(getter(samples[i]));
    }
    if (samples.size() > 1 && (samples.size() - 1) % stride != 0) {
        x.push_back(samples.back().time);
        y.push_back(getter(samples.back()));
    }

    ImPlot::PushStyleColor(ImPlotCol_Line, color);
    if (ImPlot::BeginPlot(title, ImVec2(-1.0f, 132.0f), ImPlotFlags_NoMenus | ImPlotFlags_NoTitle)) {
        ImPlot::SetupAxes("t (s)", title, ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        ImPlot::PlotLine(title, x.data(), y.data(), static_cast<int>(x.size()));
        ImPlot::EndPlot();
    }
    ImPlot::PopStyleColor();
}

} // namespace

Gui::Gui()
    : configPath_("../data/default_config.json"),
      outputPath_("../data/output.csv"),
      summaryPath_("../data/summary.json") {}

void Gui::render(PhysicsEngine& engine, Renderer2D& renderer2D, Renderer3D& renderer3D, double fps) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 origin = viewport->WorkPos;
    const ImVec2 size = viewport->WorkSize;

    const float topHeight = 42.0f;
    const float bottomHeight = std::min(280.0f, std::max(220.0f, size.y * 0.30f));
    const float leftWidth = std::min(350.0f, std::max(300.0f, size.x * 0.23f));
    const float rightWidth = std::min(430.0f, std::max(360.0f, size.x * 0.28f));
    const float centerWidth = std::max(260.0f, size.x - leftWidth - rightWidth);
    const float middleHeight = std::max(220.0f, size.y - topHeight - bottomHeight);

    drawTopBar(engine, fps, origin, ImVec2(size.x, topHeight));
    drawControls(engine, ImVec2(origin.x, origin.y + topHeight), ImVec2(leftWidth, middleHeight));
    drawCenter(engine, renderer2D, renderer3D, ImVec2(origin.x + leftWidth, origin.y + topHeight), ImVec2(centerWidth, middleHeight));
    drawCharts(engine, ImVec2(origin.x + leftWidth + centerWidth, origin.y + topHeight), ImVec2(rightWidth, middleHeight));
    drawTelemetry(engine, ImVec2(origin.x, origin.y + topHeight + middleHeight), ImVec2(size.x, bottomHeight));
    drawCreditsLicenseWindow();
}

void Gui::drawTopBar(const PhysicsEngine& engine, double fps, const ImVec2& pos, const ImVec2& size) {
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("RocketStudio Top Bar", nullptr, panelFlags | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("RocketStudio");
    ImGui::SameLine(150.0f);
    ImGui::TextDisabled("Developed by Yahia Saad");
    ImGui::SameLine(340.0f);
    ImGui::Text("Status: %s", simulationStatusName(engine.status()));
    ImGui::SameLine(500.0f);
    ImGui::Text("Integrator: %s", integratorName(engine.config().integrator));
    ImGui::SameLine(670.0f);
    ImGui::Text("FPS: %.1f", fps);
    ImGui::SameLine(780.0f);
    ImGui::Text("Time scale: %.2fx", engine.config().timeScale);
    ImGui::SameLine();
    if (ImGui::Button("Credits / License")) {
        showCreditsLicense_ = true;
    }
    if (!message_.empty()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.42f, 0.82f, 0.58f, 1.0f), "%s", message_.c_str());
    }
    ImGui::End();
}

void Gui::drawControls(PhysicsEngine& engine, const ImVec2& pos, const ImVec2& size) {
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("Simulation Controls", nullptr, panelFlags);

    SimulationConfig& config = engine.editableConfig();
    const bool running = engine.status() == SimulationStatus::Running;
    const bool paused = engine.status() == SimulationStatus::Paused;
    const bool editable = !running && !paused;

    if (ImGui::Button("Start", ImVec2(74, 0))) {
        engine.reset(config);
        engine.start();
        message_ = "Simulation started";
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause", ImVec2(74, 0))) {
        engine.pause();
    }
    ImGui::SameLine();
    if (ImGui::Button("Resume", ImVec2(82, 0))) {
        engine.resume();
    }

    if (ImGui::Button("Reset", ImVec2(74, 0))) {
        engine.reset(config);
        message_ = "Simulation reset";
    }
    ImGui::SameLine();
    if (ImGui::Button("Step", ImVec2(74, 0))) {
        engine.stepOnce();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export", ImVec2(82, 0))) {
        exportData(engine);
    }

    if (ImGui::Button("Save Config", ImVec2(116, 0))) {
        if (ConfigIO::saveConfig(configPath_, config, true)) {
            message_ = "Config saved";
        } else {
            message_ = "Config save failed";
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Config", ImVec2(116, 0))) {
        SimulationConfig loaded;
        if (ConfigIO::loadConfig(configPath_, loaded)) {
            config = loaded;
            engine.reset(config);
            message_ = "Config loaded";
        } else {
            message_ = "Default config loaded";
            config = ConfigIO::defaultConfig();
            engine.reset(config);
        }
    }

    ImGui::SeparatorText("Presets");
    for (const SimulationConfig& preset : ConfigIO::presets()) {
        if (ImGui::Button(preset.presetName.c_str(), ImVec2(-1.0f, 0.0f))) {
            config = preset;
            engine.reset(config);
            message_ = "Preset loaded: " + preset.presetName;
        }
    }

    ImGui::SeparatorText("Global");
    dragDouble("dt", config.dt, 0.001, 0.001, 2.0, "%.4f s");
    dragDouble("Time scale", config.timeScale, 0.10, 0.05, 500.0, "%.2f x");

    if (!editable) {
        ImGui::BeginDisabled();
    }

    int integratorIndex = config.integrator == IntegratorType::Euler ? 0 : 1;
    const char* integrators[] = {"Euler", "RK4"};
    if (ImGui::Combo("Integrator", &integratorIndex, integrators, 2)) {
        config.integrator = integratorIndex == 0 ? IntegratorType::Euler : IntegratorType::RK4;
    }

    dragDouble("Max time", config.maxTime, 1.0, 1.0, 20000.0, "%.1f s");
    dragDouble("Initial altitude", config.initialAltitude, 10.0, 0.0, 100000.0, "%.0f m");
    dragDouble("Initial velocity", config.initialVelocity, 1.0, 0.0, 12000.0, "%.1f m/s");
    dragDouble("Initial angle", config.initialAngleDeg, 0.1, 0.0, 90.0, "%.1f deg");
    dragDouble("Rocket dry mass", config.payloadMass, 100.0, 1.0, 1000000.0, "%.0f kg");
    dragDouble("Drag Cd", config.baseCd, 0.01, 0.0, 4.0, "%.3f");
    dragDouble("Area", config.baseArea, 0.1, 0.01, 10000.0, "%.2f m^2");
    ImGui::Checkbox("Atmosphere", &config.atmosphereEnabled);
    dragDouble("Pitch start", config.pitchStartTime, 0.1, 0.0, 1000.0, "%.1f s");
    dragDouble("Pitch rate", config.pitchRateDegPerSec, 0.01, 0.0, 5.0, "%.3f deg/s");
    dragDouble("Min pitch", config.minPitchDeg, 0.1, 0.0, 90.0, "%.1f deg");

    ImGui::SeparatorText("Stages");
    int stageCount = static_cast<int>(config.stages.size());
    if (ImGui::SliderInt("Number of stages", &stageCount, 1, 4)) {
        const int oldCount = static_cast<int>(config.stages.size());
        config.stages.resize(static_cast<size_t>(stageCount));
        for (int i = oldCount; i < stageCount; ++i) {
            config.stages[static_cast<size_t>(i)].name = "Stage " + std::to_string(i + 1);
        }
    }

    for (size_t i = 0; i < config.stages.size(); ++i) {
        Stage& stage = config.stages[i];
        if (ImGui::CollapsingHeader(stage.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushID(static_cast<int>(i));
            dragDouble("Stage dry mass", stage.dryMass, 100.0, 0.0, 1000000.0, "%.0f kg");
            dragDouble("Stage fuel mass", stage.fuelMass, 100.0, 0.0, 5000000.0, "%.0f kg");
            dragDouble("Stage thrust", stage.thrust, 10000.0, 0.0, 100000000.0, "%.0f N");
            dragDouble("Mass flow", stage.massFlowRate, 10.0, 0.0, 100000.0, "%.1f kg/s");
            dragDouble("Burn time", stage.burnTime, 1.0, 0.0, 5000.0, "%.1f s");
            dragDouble("Separation delay", stage.separationDelay, 0.1, 0.0, 120.0, "%.1f s");
            dragDouble("Cd multiplier", stage.cdMultiplier, 0.01, 0.05, 10.0, "%.2f");
            dragDouble("Area multiplier", stage.areaMultiplier, 0.01, 0.05, 10.0, "%.2f");
            ImGui::PopID();
        }
    }

    if (!editable) {
        ImGui::EndDisabled();
    }

    ImGui::End();
}

void Gui::drawCenter(PhysicsEngine& engine,
                     Renderer2D& renderer2D,
                     Renderer3D& renderer3D,
                     const ImVec2& pos,
                     const ImVec2& size) {
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("Trajectory View", nullptr, panelFlags);

    if (ImGui::BeginTabBar("ViewTabs")) {
        if (ImGui::BeginTabItem("2D Trajectory")) {
            const ImVec2 available = ImGui::GetContentRegionAvail();
            renderer2D.draw("trajectory_canvas", available, engine);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("3D View")) {
            const ImVec2 available = ImGui::GetContentRegionAvail();
            renderer3D.draw("simple_3d_canvas", available, engine);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void Gui::drawCharts(const PhysicsEngine& engine, const ImVec2& pos, const ImVec2& size) {
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("Live Charts", nullptr, panelFlags);

    const std::vector<TelemetrySample>& samples = engine.recorder().samples();
    plotMetric("Altitude", samples, [](const TelemetrySample& s) { return s.altitude; }, ImVec4(0.25f, 0.62f, 1.0f, 1.0f));
    plotMetric("Velocity", samples, [](const TelemetrySample& s) { return s.speed; }, ImVec4(0.22f, 0.82f, 0.46f, 1.0f));
    plotMetric("Acceleration", samples, [](const TelemetrySample& s) { return s.acceleration; }, ImVec4(0.96f, 0.55f, 0.20f, 1.0f));
    plotMetric("Mass", samples, [](const TelemetrySample& s) { return s.mass; }, ImVec4(0.82f, 0.78f, 0.62f, 1.0f));
    plotMetric("Drag", samples, [](const TelemetrySample& s) { return s.drag; }, ImVec4(0.95f, 0.24f, 0.24f, 1.0f));
    plotMetric("Dynamic pressure", samples, [](const TelemetrySample& s) { return s.dynamicPressure; }, ImVec4(0.95f, 0.18f, 0.16f, 1.0f));
    plotMetric("Thrust", samples, [](const TelemetrySample& s) { return s.thrust; }, ImVec4(0.68f, 0.44f, 0.95f, 1.0f));

    ImGui::End();
}

void Gui::drawTelemetry(const PhysicsEngine& engine, const ImVec2& pos, const ImVec2& size) {
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("Debug / Telemetry", nullptr, panelFlags);

    const TelemetrySample* sample = engine.recorder().latest();
    const SimulationSummary summary = engine.recorder().summary();
    const ForceBreakdown forces = engine.evaluateForces(engine.state(), engine.state().time);

    ImGui::Columns(2, "bottom_columns", true);
    if (sample && ImGui::BeginTable("telemetry_table", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg)) {
        valueRow("Time", "%.2f s", sample->time);
        valueRow("Altitude", "%.1f m", sample->altitude);
        valueRow("Downrange", "%.1f m", sample->x);
        valueRow("Velocity", "%.2f m/s", sample->speed);
        valueRow("Vertical velocity", "%.2f m/s", sample->vy);
        valueRow("Horizontal velocity", "%.2f m/s", sample->vx);
        valueRow("Acceleration", "%.3f m/s^2", sample->acceleration);
        valueRow("Gravity", "%.4f m/s^2", sample->gravity);
        valueRow("Drag", "%.2f N", sample->drag);
        valueRow("Thrust", "%.2f N", sample->thrust);
        valueRow("Dynamic pressure", "%.2f Pa", sample->dynamicPressure);
        valueRow("Mass", "%.2f kg", sample->mass);
        valueRow("Fuel remaining", "%.2f kg", sample->fuelRemaining);
        valueRow("Current stage", "%.0f", static_cast<double>(sample->currentStage + 1));
        valueRow("Max altitude", "%.1f m", summary.maxAltitude);
        valueRow("Max velocity", "%.2f m/s", summary.maxVelocity);
        valueRow("Max-Q", "%.2f Pa", summary.maxQ);
        valueRow("Escape velocity", "%.2f m/s", forces.escapeVelocity);
        valueRowText("Escape reached", summary.escapeVelocityReached ? "yes" : "no");
        ImGui::EndTable();
    }

    ImGui::NextColumn();
    drawPhysicsExplanation();
    ImGui::Columns(1);

    ImGui::End();
}

void Gui::drawPhysicsExplanation() {
    if (!ImGui::CollapsingHeader("Physics Explanation", ImGuiTreeNodeFlags_DefaultOpen)) {
        return;
    }

    ImGui::TextWrapped("Newton: F = m a. Net force changes the vehicle acceleration.");
    ImGui::TextWrapped("Acceleration: a = (T - D - m g) / m. In the simulator this is evaluated as a 2D vector.");
    ImGui::TextWrapped("Drag: D = 0.5 * rho * v^2 * Cd * A. Drag opposes the velocity vector.");
    ImGui::TextWrapped("Dynamic Pressure: q = 0.5 * rho * v^2. The maximum q value is marked as Max-Q.");
    ImGui::TextWrapped("Gravity: g(h) = (G * M) / (R + h)^2. Gravity weakens as altitude increases.");
    ImGui::TextWrapped("Atmospheric Density: rho(h) = rho0 * exp(-h / H). Density is clamped to zero above 120 km.");
    ImGui::TextWrapped("Mass Loss: m(t) = m0 - mdot * t. Each active stage burns fuel until burnout.");
    ImGui::TextWrapped("Escape Velocity: v_escape = sqrt(2GM / (R + h)). The telemetry compares speed against this value.");
}

void Gui::drawCreditsLicenseWindow() {
    if (!showCreditsLicense_) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(820.0f, 680.0f), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Credits / License", &showCreditsLicense_, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    ImGui::Text("RocketStudio");
    ImGui::TextDisabled("Developed by Yahia Saad");
    ImGui::Separator();
    ImGui::Text("Credits");
    ImGui::BulletText("Yahia Saad / First Stand Studio");
    ImGui::Bullet();
    ImGui::SameLine();
    clickableLink("GitHub: github.com/yahiawork", "https://github.com/yahiawork");
    ImGui::Bullet();
    ImGui::SameLine();
    clickableLink("Instagram: instagram.com/yahia_studio_net", "https://instagram.com/yahia_studio_net");
    ImGui::Spacing();
    ImGui::Text("License");
    ImGui::BeginChild("license_text", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::TextUnformatted(kLicenseText);
    ImGui::EndChild();
    ImGui::End();
}

void Gui::exportData(const PhysicsEngine& engine) {
    const bool csvOk = CSVWriter::write(outputPath_, engine.recorder().samples());
    const bool jsonOk = JSONWriter::writeSummary(summaryPath_, engine.recorder().summary(), engine.config());

    if (csvOk && jsonOk) {
        message_ = "Exported CSV and summary JSON";
    } else {
        message_ = "Export failed";
    }
}

} // namespace rs
