#include "Renderer2D.h"

#include <algorithm>
#include <cmath>

#include "../physics/Constants.h"
#include "../utils/MathUtils.h"

namespace rs {
namespace {

ImU32 color(float r, float g, float b, float a = 1.0f) {
    return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));
}

} // namespace

void Renderer2D::draw(const char* id, const ImVec2& size, const PhysicsEngine& engine) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const ImVec2 canvasPos = ImGui::GetCursorScreenPos();

    ImGui::InvisibleButton(id, size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    const bool hovered = ImGui::IsItemHovered();
    const bool active = ImGui::IsItemActive();

    if (hovered) {
        const float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f) {
            const double factor = wheel > 0.0f ? 1.16 : 1.0 / 1.16;
            pixelsPerMeter_ = math::clamp(pixelsPerMeter_ * factor, 0.00003, 0.25);
            followRocket_ = false;
        }
    }

    if (active && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        const ImVec2 delta = ImGui::GetIO().MouseDelta;
        viewCenter_.x -= delta.x / pixelsPerMeter_;
        viewCenter_.y += delta.y / pixelsPerMeter_;
        followRocket_ = false;
    }

    const PhysicsState& state = engine.state();
    if (followRocket_) {
        viewCenter_.x = state.position.x + 80000.0;
        viewCenter_.y = std::max(25000.0, state.position.y + 35000.0);
    }

    drawList->AddRectFilled(canvasPos, ImVec2(canvasPos.x + size.x, canvasPos.y + size.y), color(0.035f, 0.043f, 0.055f));
    drawList->AddRect(canvasPos, ImVec2(canvasPos.x + size.x, canvasPos.y + size.y), color(0.20f, 0.25f, 0.31f));
    drawGrid(drawList, canvasPos, size);

    if (hovered) {
        ImGui::SetTooltip("Left-drag to pan, mouse wheel to zoom");
    }

    const auto& samples = engine.recorder().samples();
    if (samples.size() > 1) {
        const size_t stride = std::max<size_t>(1, samples.size() / 3000);
        ImVec2 previous = worldToScreen({samples.front().x, samples.front().altitude}, canvasPos, size);
        for (size_t i = stride; i < samples.size(); i += stride) {
            const ImVec2 current = worldToScreen({samples[i].x, samples[i].altitude}, canvasPos, size);
            drawList->AddLine(previous, current, color(0.25f, 0.65f, 1.0f), 2.0f);
            previous = current;
        }
        const ImVec2 last = worldToScreen({samples.back().x, samples.back().altitude}, canvasPos, size);
        drawList->AddLine(previous, last, color(0.25f, 0.65f, 1.0f), 2.0f);
    }

    const SimulationSummary summary = engine.recorder().summary();
    if (summary.maxQ > 0.0) {
        auto it = std::min_element(samples.begin(), samples.end(), [&](const TelemetrySample& a, const TelemetrySample& b) {
            return std::abs(a.time - summary.maxQTime) < std::abs(b.time - summary.maxQTime);
        });
        if (it != samples.end()) {
            const ImVec2 p = worldToScreen({it->x, it->altitude}, canvasPos, size);
            drawList->AddCircleFilled(p, 6.0f, color(0.95f, 0.18f, 0.16f));
            drawList->AddText(ImVec2(p.x + 8.0f, p.y - 18.0f), color(1.0f, 0.42f, 0.38f), "Max-Q");
        }
    }

    for (const StageEvent& event : engine.recorder().stageEvents()) {
        const ImVec2 p = worldToScreen({event.x, event.altitude}, canvasPos, size);
        drawList->AddCircleFilled(p, 5.0f, color(0.67f, 0.43f, 0.95f));
        drawList->AddLine(ImVec2(p.x, canvasPos.y), ImVec2(p.x, canvasPos.y + size.y), color(0.67f, 0.43f, 0.95f, 0.35f), 1.0f);
        drawList->AddText(ImVec2(p.x + 7.0f, p.y + 5.0f), color(0.78f, 0.60f, 1.0f), event.name.c_str());
    }

    const ImVec2 rocket = worldToScreen(state.position, canvasPos, size);
    const double speed = state.velocity.length();
    const double heading = speed > 1.0 ? std::atan2(state.velocity.y, state.velocity.x)
                                       : math::degToRad(engine.pitchAngleDeg(state.time));
    const float rocketLength = 20.0f;
    const ImVec2 nose(rocket.x + std::cos(heading) * rocketLength, rocket.y - std::sin(heading) * rocketLength);
    const ImVec2 left(rocket.x + std::cos(heading + 2.45) * 11.0f, rocket.y - std::sin(heading + 2.45) * 11.0f);
    const ImVec2 right(rocket.x + std::cos(heading - 2.45) * 11.0f, rocket.y - std::sin(heading - 2.45) * 11.0f);
    drawList->AddTriangleFilled(nose, left, right, color(0.93f, 0.95f, 0.98f));
    drawList->AddCircleFilled(rocket, 4.0f, color(0.16f, 0.82f, 0.54f));

    const ImVec2 velocityEnd(rocket.x + static_cast<float>(state.velocity.x * 0.08),
                             rocket.y - static_cast<float>(state.velocity.y * 0.08));
    drawArrow(drawList, rocket, velocityEnd, color(0.20f, 0.80f, 0.45f), 2.0f);

    const ImVec2 accelerationEnd(rocket.x + static_cast<float>(state.acceleration.x * 8.0),
                                 rocket.y - static_cast<float>(state.acceleration.y * 8.0));
    drawArrow(drawList, rocket, accelerationEnd, color(0.95f, 0.55f, 0.20f), 2.0f);

    const ImVec2 altitudeBase = worldToScreen({state.position.x, 0.0}, canvasPos, size);
    drawList->AddLine(altitudeBase, rocket, color(0.72f, 0.80f, 0.90f, 0.35f), 1.0f);

    ImGui::SetCursorScreenPos(ImVec2(canvasPos.x + 8.0f, canvasPos.y + 8.0f));
    ImGui::BeginGroup();
    if (ImGui::SmallButton(followRocket_ ? "Following" : "Follow")) {
        followRocket_ = true;
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Reset View")) {
        pixelsPerMeter_ = 0.0014;
        followRocket_ = true;
    }
    ImGui::SameLine();
    ImGui::Text("Scale %.3f px/km", pixelsPerMeter_ * 1000.0);
    ImGui::EndGroup();
}

ImVec2 Renderer2D::worldToScreen(const Vec2& world, const ImVec2& origin, const ImVec2& size) const {
    return ImVec2(
        origin.x + size.x * 0.5f + static_cast<float>((world.x - viewCenter_.x) * pixelsPerMeter_),
        origin.y + size.y * 0.5f - static_cast<float>((world.y - viewCenter_.y) * pixelsPerMeter_));
}

Vec2 Renderer2D::screenToWorld(const ImVec2& screen, const ImVec2& origin, const ImVec2& size) const {
    return {
        viewCenter_.x + (screen.x - origin.x - size.x * 0.5f) / pixelsPerMeter_,
        viewCenter_.y - (screen.y - origin.y - size.y * 0.5f) / pixelsPerMeter_};
}

void Renderer2D::drawGrid(ImDrawList* drawList, const ImVec2& origin, const ImVec2& size) const {
    const Vec2 minWorld = screenToWorld(ImVec2(origin.x, origin.y + size.y), origin, size);
    const Vec2 maxWorld = screenToWorld(ImVec2(origin.x + size.x, origin.y), origin, size);
    const double xStep = math::niceGridStep(maxWorld.x - minWorld.x, 10);
    const double yStep = math::niceGridStep(maxWorld.y - minWorld.y, 8);

    const ImU32 gridColor = color(0.16f, 0.20f, 0.25f, 0.55f);
    const ImU32 axisColor = color(0.40f, 0.47f, 0.55f, 0.90f);

    const double firstX = std::floor(minWorld.x / xStep) * xStep;
    for (double x = firstX; x <= maxWorld.x; x += xStep) {
        const ImVec2 a = worldToScreen({x, minWorld.y}, origin, size);
        const ImVec2 b = worldToScreen({x, maxWorld.y}, origin, size);
        drawList->AddLine(a, b, std::abs(x) < 1.0 ? axisColor : gridColor, std::abs(x) < 1.0 ? 2.0f : 1.0f);
    }

    const double firstY = std::floor(minWorld.y / yStep) * yStep;
    for (double y = firstY; y <= maxWorld.y; y += yStep) {
        const ImVec2 a = worldToScreen({minWorld.x, y}, origin, size);
        const ImVec2 b = worldToScreen({maxWorld.x, y}, origin, size);
        drawList->AddLine(a, b, std::abs(y) < 1.0 ? axisColor : gridColor, std::abs(y) < 1.0 ? 2.0f : 1.0f);
    }

    ImVec2 previous = worldToScreen({minWorld.x, -minWorld.x * minWorld.x / (2.0 * constants::earthRadius)}, origin, size);
    for (int i = 1; i <= 160; ++i) {
        const double t = static_cast<double>(i) / 160.0;
        const double x = minWorld.x + (maxWorld.x - minWorld.x) * t;
        const double y = -x * x / (2.0 * constants::earthRadius);
        const ImVec2 current = worldToScreen({x, y}, origin, size);
        drawList->AddLine(previous, current, color(0.36f, 0.56f, 0.40f), 2.0f);
        previous = current;
    }
}

void Renderer2D::drawArrow(ImDrawList* drawList, const ImVec2& from, const ImVec2& to, ImU32 colorValue, float thickness) const {
    drawList->AddLine(from, to, colorValue, thickness);
    const float dx = to.x - from.x;
    const float dy = to.y - from.y;
    const float len = std::sqrt(dx * dx + dy * dy);
    if (len < 5.0f) {
        return;
    }
    const float ux = dx / len;
    const float uy = dy / len;
    const ImVec2 left(to.x - ux * 9.0f - uy * 5.0f, to.y - uy * 9.0f + ux * 5.0f);
    const ImVec2 right(to.x - ux * 9.0f + uy * 5.0f, to.y - uy * 9.0f - ux * 5.0f);
    drawList->AddTriangleFilled(to, left, right, colorValue);
}

} // namespace rs
