#include "Renderer3D.h"

#include <algorithm>
#include <cmath>

namespace rs {
namespace {

ImU32 c(float r, float g, float b, float a = 1.0f) {
    return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));
}

} // namespace

void Renderer3D::draw(const char* id, const ImVec2& size, const PhysicsEngine& engine) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton(id, size);

    drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), c(0.030f, 0.036f, 0.048f));
    drawList->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), c(0.18f, 0.22f, 0.27f));

    const ImVec2 center(pos.x + size.x * 0.50f, pos.y + size.y * 0.58f);
    const float radius = std::min(size.x, size.y) * 0.32f;

    drawList->AddCircleFilled(center, radius, c(0.05f, 0.25f, 0.42f));
    drawList->AddCircle(center, radius, c(0.28f, 0.60f, 0.82f), 96, 2.0f);
    drawList->AddCircleFilled(ImVec2(center.x - radius * 0.35f, center.y - radius * 0.25f), radius * 0.16f, c(0.10f, 0.45f, 0.28f, 0.75f));
    drawList->AddCircleFilled(ImVec2(center.x + radius * 0.25f, center.y + radius * 0.18f), radius * 0.20f, c(0.10f, 0.42f, 0.25f, 0.70f));

    const auto& samples = engine.recorder().samples();
    if (samples.size() > 1) {
        double maxX = 1.0;
        double maxY = 1.0;
        for (const TelemetrySample& sample : samples) {
            maxX = std::max(maxX, std::abs(sample.x));
            maxY = std::max(maxY, sample.altitude);
        }

        const size_t stride = std::max<size_t>(1, samples.size() / 400);
        ImVec2 previous(center.x, center.y - radius);
        bool first = true;
        for (size_t i = 0; i < samples.size(); i += stride) {
            const TelemetrySample& sample = samples[i];
            const float px = center.x + static_cast<float>((sample.x / maxX) * radius * 1.35);
            const float py = center.y - radius - static_cast<float>((sample.altitude / maxY) * radius * 0.95);
            const ImVec2 current(px, py);
            if (!first) {
                drawList->AddLine(previous, current, c(0.35f, 0.75f, 1.0f), 2.0f);
            }
            previous = current;
            first = false;
        }
    }

    const PhysicsState& state = engine.state();
    const ImVec2 rocket(center.x + static_cast<float>(std::cos(state.position.x / 250000.0) * radius * 0.12),
                        center.y - radius - static_cast<float>(std::min(1.0, state.position.y / 250000.0) * radius));
    drawList->AddCircleFilled(rocket, 5.0f, c(0.95f, 0.95f, 0.98f));
    drawList->AddText(ImVec2(pos.x + 10.0f, pos.y + 8.0f), c(0.86f, 0.90f, 0.95f), "Simple 3D mission view");
}

} // namespace rs
