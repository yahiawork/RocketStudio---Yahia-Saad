# 20-Minute Video Intro Script

## Opening

Welcome to RocketStudio, an educational launch simulation suite built in modern C++. In this tutorial series, we are going to build a full desktop studio app, not a tiny console demo. The goal is to create a complete simulator with a physics engine, real-time visualization, live charts, telemetry, CSV and JSON export, Python analysis, and a clean architecture that can grow.

Before we start, one important note: this is a software simulation only. We are not building real rockets, real engines, real propellants, or launch hardware. This is an educational approximation, not an official NASA-grade mission simulator. The project is inspired by the style of large staged launch vehicles, but the physics are simplified so we can learn programming, numerical methods, and simulation design.

## What the Simulator Does

RocketStudio tracks a vehicle in two dimensions. The horizontal axis is downrange distance, and the vertical axis is altitude. At each time step, the simulator knows the rocket position, velocity, acceleration, current mass, fuel remaining, active stage, thrust, drag, gravity, density, and dynamic pressure.

The desktop app gives us a studio-style interface. On the left, we have simulation controls and editable parameters. In the center, we see a trajectory view. On the right, we have live graphs. At the bottom, we have telemetry and an educational formula panel. This means we are not only calculating numbers; we are building a tool that explains what it is doing.

## Why Physics Matters

Simulation is about cause and effect. If thrust is greater than weight and drag, the vehicle accelerates upward. If drag grows because speed and atmospheric density are high, acceleration can drop. If a stage burns out, mass changes. If the vehicle climbs high enough, gravity becomes slightly weaker and atmospheric density falls.

Even with a simplified model, these relationships produce interesting behavior. You can see Max-Q, staging, coast periods, and the difference between Euler and RK4 integration. The point is not to reproduce a real mission perfectly. The point is to understand how a simulation loop turns equations into motion.

## Newton's Law

The central equation is Newton's second law:

```text
F = m a
```

Force equals mass times acceleration. In code, we usually rearrange that:

```text
a = F / m
```

At every simulation step, we compute all the forces acting on the vehicle, add them together, divide by mass, and get acceleration. Then the integrator uses acceleration to update velocity, and velocity to update position.

## Thrust

Thrust is the force produced by the active stage in the simulation. The thrust has a magnitude and a direction. The magnitude comes from the stage configuration. The direction comes from a simple pitch program.

At launch, the rocket points mostly upward. After a configurable start time, the pitch angle gradually decreases toward horizontal. This creates an educational gravity-turn-like trajectory. It is deliberately simple, stable, and easy to inspect.

## Gravity

Gravity is not constant in RocketStudio. We use:

```text
g(h) = (G * M) / (R + h)^2
```

Here, `h` is altitude. As altitude increases, the distance from Earth's center increases, so gravity decreases. Near the ground the value is close to standard gravity. Higher up, it becomes a little smaller.

Gravity always points downward in our 2D coordinate system.

## Atmospheric Density

Atmospheric density matters because drag depends on it. We use an exponential atmosphere:

```text
rho(h) = rho0 * exp(-h / H)
```

At sea level, density is highest. As altitude increases, density decreases quickly. Above 120 km, we clamp density to zero. This is a simplified model, but it is good enough to make drag and Max-Q visible.

## Drag

Drag is:

```text
D = 0.5 * rho * v^2 * Cd * A
```

That formula tells us several important things. If density is high, drag is high. If speed doubles, the `v^2` term makes drag grow by four times. If the vehicle has a larger cross-sectional area or drag coefficient, drag also grows.

Drag always points opposite the velocity vector. If the rocket is moving upward and right, drag points downward and left. If speed is zero, drag is zero.

## Dynamic Pressure and Max-Q

Dynamic pressure is:

```text
q = 0.5 * rho * v^2
```

Max-Q is the maximum dynamic pressure during the flight. It usually happens when the vehicle is moving fast but still inside enough atmosphere for density to matter. In the app, Max-Q is tracked, displayed in telemetry, exported to JSON, and marked on the 2D view.

## Fuel Burn and Mass

A launch vehicle gets lighter as fuel burns. In RocketStudio, each stage has:

- Dry mass.
- Fuel mass.
- Thrust.
- Mass flow rate.
- Burn time.
- Separation delay.

The active stage burns fuel according to:

```text
fuel_next = fuel_current - mdot * dt
```

Fuel is clamped so it never goes negative. Total mass includes payload mass, attached dry stage masses, and remaining fuel.

## Staging

When a stage finishes burning, RocketStudio records a stage separation event. The dry mass of that spent stage is removed from the vehicle. The next stage becomes active after a short configurable delay.

This is one of the most important concepts in launch simulation: staging changes the vehicle mass dramatically, which changes acceleration. You can see this in the live mass chart and in the trajectory.

## Numerical Integration

The simulator supports Euler and RK4.

Euler is direct and easy:

```text
state_next = state + derivative * dt
```

It is useful for learning, but it can become inaccurate when the time step is large.

RK4, or fourth-order Runge-Kutta, samples the derivative four times during a time step:

```text
k1 = f(state, t)
k2 = f(state + k1*dt/2, t + dt/2)
k3 = f(state + k2*dt/2, t + dt/2)
k4 = f(state + k3*dt, t + dt)
```

Then it blends those samples. RK4 usually gives smoother and more accurate results. RocketStudio lets you switch between them and compare.

## What the Viewer Will Build

In this series, we will build:

1. A CMake project for MSYS2 UCRT64.
2. A clean C++ physics engine.
3. Stage and fuel systems.
4. Euler and RK4 integrators.
5. A simulation recorder.
6. CSV and JSON export.
7. A Dear ImGui desktop interface.
8. A 2D trajectory renderer.
9. A simple 3D mission view.
10. Live ImPlot graphs.
11. Python plotting and replay tools.
12. Internal validation tests.
13. Documentation and tutorial material.

By the end, we will have a complete local application that can run in GUI mode, run in headless mode, export data, plot results, and teach the physics as it runs.

## Closing

The best part of a project like this is that it connects many areas of programming: numerical methods, graphics, UI design, data export, file formats, testing, and documentation. We will keep the physics honest, the architecture clean, and the app practical. Let's build RocketStudio.
