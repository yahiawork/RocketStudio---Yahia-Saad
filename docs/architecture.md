# Architecture

RocketStudio is split into small modules so physics, rendering, UI, and file IO do not depend on each other unnecessarily.

## app

The `app` module owns the desktop experience.

- `App` creates the GLFW window, OpenGL context, ImGui context, and ImPlot context.
- `Gui` builds the studio dashboard.
- `Renderer2D` draws the trajectory canvas using ImGui draw lists.
- `Renderer3D` draws a simple 3D-style mission view.
- `Theme` applies the dark engineering dashboard style.

## physics

The `physics` module owns the simulation model.

- `Vec2` is the 2D vector type.
- `Constants` stores physical constants.
- `Stage` describes one stage.
- `Rocket` tracks stage fuel, dry mass, active stage, burn, and separation.
- `Environment` computes gravity, density, dynamic pressure, drag, and escape velocity.
- `PhysicsState` stores time, position, velocity, and acceleration.
- `Integrator` implements Euler and RK4.
- `PhysicsEngine` coordinates forces, integration, staging, finish rules, and recording.
- `SimulationRecorder` stores samples and summary metrics.

## io

The `io` module owns persistence and export.

- `ConfigIO` loads and saves JSON configuration files and provides built-in presets.
- `CSVWriter` exports telemetry samples.
- `JSONWriter` exports the summary file.

The project uses a small internal JSON parser for the configuration format so the simulator can run without a Python or Node runtime.

## utils

The `utils` module contains shared support code.

- `MathUtils` handles clamping, angle conversion, finite checks, and grid spacing.
- `Logger` writes simple console messages.
- `Timer` measures frame time.

## Data Flow

1. The GUI edits a `SimulationConfig`.
2. `PhysicsEngine` resets a `Rocket` from that config.
3. Each simulation step evaluates forces through `Environment` and `Rocket`.
4. The chosen integrator advances `PhysicsState`.
5. `Rocket` burns fuel and emits stage events.
6. `SimulationRecorder` stores telemetry samples and summary values.
7. Renderers and charts read recorder data.
8. IO writers export CSV and JSON.

## Headless Flow

CLI mode bypasses the GUI:

```text
main -> ConfigIO -> PhysicsEngine.runToEnd -> CSVWriter + JSONWriter
```

This makes the project usable for scripts, tests, and video tutorial automation.
