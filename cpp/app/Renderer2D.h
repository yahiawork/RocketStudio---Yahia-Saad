#pragma once

#include <imgui.h>

#include "../physics/PhysicsEngine.h"

namespace rs {

class Renderer2D {
public:
    void draw(const char* id, const ImVec2& size, const PhysicsEngine& engine);

private:
    double pixelsPerMeter_ = 0.0014;
    Vec2 viewCenter_{80000.0, 60000.0};
    bool followRocket_ = true;

    ImVec2 worldToScreen(const Vec2& world, const ImVec2& origin, const ImVec2& size) const;
    Vec2 screenToWorld(const ImVec2& screen, const ImVec2& origin, const ImVec2& size) const;
    void drawGrid(ImDrawList* drawList, const ImVec2& origin, const ImVec2& size) const;
    void drawArrow(ImDrawList* drawList, const ImVec2& from, const ImVec2& to, ImU32 color, float thickness) const;
};

} // namespace rs
