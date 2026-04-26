#pragma once

#include "Vec2.h"

namespace rs {

struct PhysicsState {
    double time = 0.0;
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
};

} // namespace rs
