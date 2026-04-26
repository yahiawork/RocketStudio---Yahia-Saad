#pragma once

#include <functional>

#include "PhysicsState.h"

namespace rs {

struct StateDerivative {
    Vec2 dPosition;
    Vec2 dVelocity;
};

using DerivativeFunction = std::function<StateDerivative(const PhysicsState&, double)>;

class Integrator {
public:
    static PhysicsState euler(const PhysicsState& state, double dt, const DerivativeFunction& derivative);
    static PhysicsState rk4(const PhysicsState& state, double dt, const DerivativeFunction& derivative);

private:
    static PhysicsState addScaled(const PhysicsState& state, const StateDerivative& derivative, double scale);
};

} // namespace rs
