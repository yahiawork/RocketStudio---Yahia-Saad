# Usage

## Start the GUI

Build the project, then run:

```sh
cd build
./RocketStudio.exe
```

## Controls

- `Start`: reset using the current configuration and begin running.
- `Pause`: pause the simulation.
- `Resume`: continue after a pause.
- `Reset`: reset to time zero.
- `Step`: advance one simulation step.
- `Export`: write `data/output.csv` and `data/summary.json`.
- `Save Config`: write the current settings to `data/default_config.json`.
- `Load Config`: load `data/default_config.json`.

## Presets

The left panel includes:

- Small Educational Rocket
- Heavy Artemis-like Rocket
- High Drag Test
- No Atmosphere Test
- Euler vs RK4 Test

Loading a preset resets the simulation.

## Trajectory View

The 2D view shows:

- Ground line.
- Earth curvature approximation.
- Rocket marker.
- Flight trail.
- Velocity vector.
- Acceleration vector.
- Altitude marker.
- Max-Q marker.
- Stage separation markers.

Mouse controls:

- Left-drag to pan.
- Mouse wheel to zoom.
- `Follow` to recenter on the rocket.
- `Reset View` to restore the default scale.

## Charts

The right panel plots:

- Altitude vs time.
- Velocity vs time.
- Acceleration vs time.
- Mass vs time.
- Drag vs time.
- Dynamic pressure vs time.
- Thrust vs time.

## Telemetry

The bottom panel reports:

- Time.
- Altitude.
- Downrange distance.
- Velocity.
- Vertical and horizontal velocity.
- Acceleration.
- Gravity.
- Drag.
- Thrust.
- Dynamic pressure.
- Mass.
- Fuel remaining.
- Current stage.
- Max altitude.
- Max velocity.
- Max-Q.
- Escape velocity.
- Escape-velocity reached flag.

## Headless Mode

```sh
./RocketStudio.exe --headless --config ../data/default_config.json --output ../data/output.csv --summary ../data/summary.json
```

## Validation Mode

```sh
./RocketStudio.exe --test
```

The tests check gravity, density, zero-speed drag, non-negative mass and fuel, and whether Euler and RK4 run without NaN values.
