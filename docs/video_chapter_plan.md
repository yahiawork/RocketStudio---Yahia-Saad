# 5 to 10 Hour Tutorial Chapter Plan

## 1. Introduction

- Project goals.
- Safety and simulation-only scope.
- What the app will look like.
- What "educational approximation" means.

Estimated time: 20 to 35 minutes.

## 2. Project Structure

- Folder layout.
- CMake entry point.
- Module boundaries.
- Data and docs folders.

Estimated time: 25 to 45 minutes.

## 3. Physics Model

- 2D state.
- Forces.
- Gravity.
- Atmosphere.
- Drag.
- Dynamic pressure.
- Escape velocity.

Estimated time: 45 to 70 minutes.

## 4. C++ Engine

- `Vec2`.
- `PhysicsState`.
- `Environment`.
- `PhysicsEngine`.
- State update flow.

Estimated time: 45 to 75 minutes.

## 5. Integrators

- Euler.
- RK4.
- Derivative functions.
- Comparing accuracy and stability.

Estimated time: 35 to 60 minutes.

## 6. Stage System

- Stage data.
- Fuel burn.
- Mass tracking.
- Separation events.
- Stage delay.

Estimated time: 45 to 75 minutes.

## 7. Data Recording

- Telemetry samples.
- Summary values.
- Max altitude.
- Max velocity.
- Max-Q.
- Escape-velocity flag.

Estimated time: 30 to 50 minutes.

## 8. GUI Setup

- GLFW window.
- OpenGL context.
- Dear ImGui setup.
- ImPlot setup.
- Theme.

Estimated time: 45 to 80 minutes.

## 9. 2D Rendering

- Coordinate transforms.
- Grid.
- Ground and Earth curvature.
- Trail drawing.
- Rocket marker.
- Velocity and acceleration vectors.
- Max-Q and stage markers.
- Pan and zoom.

Estimated time: 60 to 90 minutes.

## 10. Live Telemetry

- Bottom panel layout.
- Real-time values.
- Escape velocity display.
- Educational formula panel.

Estimated time: 30 to 50 minutes.

## 11. Charts

- ImPlot basics.
- Downsampling samples for rendering.
- Altitude chart.
- Velocity chart.
- Acceleration chart.
- Mass, drag, dynamic pressure, and thrust charts.

Estimated time: 35 to 60 minutes.

## 12. Export System

- CSV writer.
- JSON summary writer.
- Config save/load.
- Presets.
- Missing config recovery.

Estimated time: 45 to 75 minutes.

## 13. Python Analysis

- Installing requirements.
- Reading CSV with pandas.
- Plotting with matplotlib.
- Summary analysis.
- Trajectory replay animation.

Estimated time: 35 to 60 minutes.

## 14. Testing

- Internal `--test` mode.
- Gravity test.
- Density test.
- Zero-speed drag test.
- Non-negative mass and fuel.
- Euler and RK4 NaN checks.

Estimated time: 25 to 45 minutes.

## 15. Final Polish

- README.
- Troubleshooting.
- Headless mode.
- `run.bat`, `run_msys.sh`, and `run_all.py`.
- UI spacing and readability.
- Final build and demo.

Estimated time: 45 to 90 minutes.

## Suggested Total Runtime

- Fast version: 5 hours.
- Normal version: 7 hours.
- Deep explanation version: 9 to 10 hours.
