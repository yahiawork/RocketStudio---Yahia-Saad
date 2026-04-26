#pragma once

#include <imgui.h>

#include "../physics/PhysicsEngine.h"

namespace rs {

class Renderer3D {
public:
    void draw(const char* id, const ImVec2& size, const PhysicsEngine& engine);
};

} // namespace rs
