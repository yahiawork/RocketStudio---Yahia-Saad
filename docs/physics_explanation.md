# Physics Explanation

RocketStudio is an educational approximation of a staged launch. The simulator tracks a vehicle in two dimensions:

- `x`: downrange distance in meters.
- `y`: altitude in meters.
- `velocity`: horizontal and vertical velocity.
- `acceleration`: horizontal and vertical acceleration.

## Newton's Second Law

Newton's second law is the foundation:

```text
F = m a
```

The simulator computes the net force on the vehicle and divides by current mass to get acceleration.

## Thrust

Thrust is modeled as a force vector. Its magnitude comes from the active stage. Its direction comes from a simple pitch program:

- The launch begins mostly vertical.
- After `pitchStartTime`, the angle decreases at `pitchRateDegPerSec`.
- The pitch angle never goes below `minPitchDeg`.

This is not a real guidance system. It is a stable educational model for visualizing gravity turn behavior.

## Gravity

Gravity changes with altitude:

```text
g(h) = (G * M) / (R + h)^2
```

Where:

- `G` is the gravitational constant.
- `M` is Earth's mass.
- `R` is Earth's mean radius.
- `h` is altitude.

The gravity force always points downward.

## Drag

Drag is computed from:

```text
D = 0.5 * rho * v^2 * Cd * A
```

Where:

- `rho` is atmospheric density.
- `v` is speed.
- `Cd` is drag coefficient.
- `A` is cross-sectional area.

Drag is zero when velocity is zero and always points opposite the velocity vector.

## Atmospheric Density

The atmosphere uses an exponential model:

```text
rho(h) = rho0 * exp(-h / H)
```

Density is clamped to zero above 120 km. This keeps the model stable and avoids pretending to capture the full complexity of atmospheric layers.

## Dynamic Pressure and Max-Q

Dynamic pressure is:

```text
q = 0.5 * rho * v^2
```

Max-Q is the maximum dynamic pressure encountered during flight. RocketStudio records:

- Max-Q value.
- Time.
- Altitude.
- Velocity.

The 2D view marks Max-Q with a red marker.

## Fuel Burn

Each stage has a fuel mass and a mass flow rate:

```text
fuel_next = max(0, fuel_current - mdot * dt)
```

The total vehicle mass decreases as fuel burns. Dry mass is retained until stage separation.

## Staging

When a stage runs out of fuel or reaches its burn-time limit:

- A stage separation event is recorded.
- The spent dry mass is dropped.
- The next stage becomes active after the configured separation delay.

Events are exported to `summary.json`.

## Numerical Integration

RocketStudio supports Euler and RK4.

Euler is simple:

```text
state_next = state + derivative * dt
```

RK4 samples the derivative four times:

```text
k1 = f(state, t)
k2 = f(state + k1*dt/2, t + dt/2)
k3 = f(state + k2*dt/2, t + dt/2)
k4 = f(state + k3*dt, t + dt)
state_next = state + dt/6 * (k1 + 2k2 + 2k3 + k4)
```

RK4 usually produces smoother trajectories at larger time steps, but no integrator makes this simplified model fully realistic.
