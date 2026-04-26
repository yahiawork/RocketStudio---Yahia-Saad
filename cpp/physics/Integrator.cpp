#include "Integrator.h"

namespace rs {

PhysicsState Integrator::addScaled(const PhysicsState& state, const StateDerivative& derivative, double scale) {
    PhysicsState result = state;
    result.position += derivative.dPosition * scale;
    result.velocity += derivative.dVelocity * scale;
    result.time += scale;
    return result;
}

PhysicsState Integrator::euler(const PhysicsState& state, double dt, const DerivativeFunction& derivative) {
    const StateDerivative k1 = derivative(state, state.time);
    PhysicsState result = state;
    result.position += k1.dPosition * dt;
    result.velocity += k1.dVelocity * dt;
    result.time += dt;
    result.acceleration = k1.dVelocity;
    return result;
}

PhysicsState Integrator::rk4(const PhysicsState& state, double dt, const DerivativeFunction& derivative) {
    const StateDerivative k1 = derivative(state, state.time);
    const StateDerivative k2 = derivative(addScaled(state, k1, dt * 0.5), state.time + dt * 0.5);
    const StateDerivative k3 = derivative(addScaled(state, k2, dt * 0.5), state.time + dt * 0.5);
    const StateDerivative k4 = derivative(addScaled(state, k3, dt), state.time + dt);

    PhysicsState result = state;
    result.position += (k1.dPosition + 2.0 * k2.dPosition + 2.0 * k3.dPosition + k4.dPosition) * (dt / 6.0);
    result.velocity += (k1.dVelocity + 2.0 * k2.dVelocity + 2.0 * k3.dVelocity + k4.dVelocity) * (dt / 6.0);
    result.time += dt;
    result.acceleration = (k1.dVelocity + 2.0 * k2.dVelocity + 2.0 * k3.dVelocity + k4.dVelocity) / 6.0;
    return result;
}

} // namespace rs
