# RocketStudio - Artemis-like Launch Simulation Suite

RocketStudio is a local desktop studio application for educational rocket launch simulation. It combines a C++ physics engine, a Dear ImGui dashboard, OpenGL/GLFW rendering, live ImPlot charts, CSV/JSON export, and Python analysis tools.

Developed by Yahia Saad / First Stand Studio.

This is a software simulation only. It does not include instructions for building real rockets, propellants, explosives, engines, fuel mixtures, or launch hardware.

This is an educational approximation, not an official NASA-grade mission simulator.

## What It Does

- Simulates a 2D launch trajectory using position, velocity, and acceleration vectors.
- Models varying gravity, exponential atmosphere, aerodynamic drag, dynamic pressure, Max-Q, staged fuel burn, and stage separation.
- Supports Euler and RK4 numerical integration.
- Provides a dark engineering-dashboard GUI with controls, telemetry, charts, trajectory rendering, and an educational formula panel.
- Exports `data/output.csv` and `data/summary.json`.
- Runs in GUI mode or headless CLI mode.
- Includes Python scripts for plotting, analysis, and replay.

## What It Is Not

- Not an official NASA, ESA, SpaceX, or aerospace-industry simulator.
- Not a real mission planning system.
- Not a hardware design tool.
- Not guidance for physical rocket construction.
- Not a substitute for professional aerospace validation.

## Credits

Developed by Yahia Saad / First Stand Studio.

- GitHub: https://github.com/yahiawork
- Instagram: https://instagram.com/yahia_studio_net

## License

RocketStudio is licensed under the First Stand Studio Proprietary License v1.0.

Copyright (c) 2026 Yahia Saad / First Stand Studio. All rights reserved.

See [LICENSE.md](LICENSE.md) for the full license text.

## Screenshots

After launching the app, the first screen shows a studio-style dashboard:

- Left: simulation controls, presets, integrator settings, and stage parameters.
- Center: live 2D trajectory view with ground, Earth curvature approximation, rocket marker, trail, vectors, Max-Q marker, and stage markers.
- Right: live plots for altitude, velocity, acceleration, mass, drag, dynamic pressure, and thrust.
- Bottom: telemetry and the physics explanation panel.

## MSYS2 UCRT64 Setup

Open the MSYS2 UCRT64 terminal and run:

```sh
pacman -Syu
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-ninja
pacman -S mingw-w64-ucrt-x86_64-glfw
pacman -S mingw-w64-ucrt-x86_64-glew
pacman -S mingw-w64-ucrt-x86_64-python
pacman -S mingw-w64-ucrt-x86_64-python-pip
```

Dear ImGui and ImPlot are fetched by CMake from lightweight release archives if they are not installed as system packages.

## Build

From the `RocketStudio` directory:

```sh
mkdir build
cd build
cmake -G Ninja ../cpp
ninja
./RocketStudio.exe
```

Windows Command Prompt users can run:

```bat
run.bat
```

MSYS2 shell users can run:

```sh
./run_msys.sh
```

## CLI Usage

Run a full simulation without the GUI:

```sh
./RocketStudio.exe --headless --config ../data/default_config.json --output ../data/output.csv --summary ../data/summary.json
```

Run internal validation checks:

```sh
./RocketStudio.exe --test
```

Show command help:

```sh
./RocketStudio.exe --help
```

## Python Plotting

Install Python dependencies:

```sh
pip install -r requirements.txt
```

Generate plots:

```sh
python python_tools/plot_results.py
```

Analyze results:

```sh
python python_tools/analyze_results.py
```

Replay the trajectory:

```sh
python python_tools/replay_csv.py
```

Build, run headless, plot, and analyze in one command:

```sh
python run_all.py
```

## Project Structure

```text
RocketStudio/
  README.md
  requirements.txt
  run.bat
  run_msys.sh
  run_all.py
  cpp/
    CMakeLists.txt
    main.cpp
    app/
    physics/
    io/
    utils/
  python_tools/
  data/
  docs/
  assets/
```

## Physics Assumptions

- 2D motion uses `x` for downrange distance and `y` for altitude.
- Earth is approximated by a fixed radius and mass.
- Gravity varies with altitude using Newtonian inverse-square gravity.
- Atmosphere uses a simple exponential density model and is clamped above 120 km.
- Drag opposes velocity and depends on density, speed, drag coefficient, and cross-sectional area.
- Thrust follows a simple pitch program that gradually tilts from vertical toward horizontal.
- Staging drops dry mass after a stage burns out.
- RK4 improves numerical integration quality, but the model is still simplified and educational.

## Troubleshooting

- If CMake cannot find GLFW, confirm you are inside the MSYS2 UCRT64 terminal and installed `mingw-w64-ucrt-x86_64-glfw`.
- If CMake cannot download ImGui or ImPlot, check internet access and retry the configure step.
- If `./RocketStudio.exe` fails to start from a different folder, run it from the `build` directory or use `run.bat`.
- If Python plotting fails, run `pip install -r requirements.txt`.
- If the GUI appears slow, lower the time scale or increase `dt` slightly.
- If a config file is missing, RocketStudio creates a default config automatically.
