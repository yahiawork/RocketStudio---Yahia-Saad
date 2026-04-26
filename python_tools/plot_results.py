from __future__ import annotations

from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT / "data"
CSV = DATA / "output.csv"


def make_plot(df: pd.DataFrame, column: str, title: str, ylabel: str, filename: str) -> None:
    fig, ax = plt.subplots(figsize=(10, 5))
    ax.plot(df["time"], df[column], linewidth=1.8)
    ax.set_title(title)
    ax.set_xlabel("Time (s)")
    ax.set_ylabel(ylabel)
    ax.grid(True, alpha=0.35)
    fig.tight_layout()
    fig.savefig(DATA / filename, dpi=140)
    plt.close(fig)


def main() -> int:
    if not CSV.exists():
        print(f"Missing {CSV}. Run RocketStudio headless or export from the GUI first.")
        return 1

    DATA.mkdir(exist_ok=True)
    df = pd.read_csv(CSV)
    make_plot(df, "altitude", "Altitude vs Time", "Altitude (m)", "altitude_vs_time.png")
    make_plot(df, "speed", "Velocity vs Time", "Speed (m/s)", "velocity_vs_time.png")
    make_plot(df, "acceleration", "Acceleration vs Time", "Acceleration (m/s^2)", "acceleration_vs_time.png")
    make_plot(df, "mass", "Mass vs Time", "Mass (kg)", "mass_vs_time.png")
    make_plot(df, "dynamic_pressure", "Dynamic Pressure vs Time", "q (Pa)", "dynamic_pressure_vs_time.png")

    print(f"Plots written to {DATA}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
