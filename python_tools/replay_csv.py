from __future__ import annotations

from pathlib import Path

import matplotlib.animation as animation
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


ROOT = Path(__file__).resolve().parents[1]
CSV = ROOT / "data" / "output.csv"


def main() -> int:
    if not CSV.exists():
        print(f"Missing {CSV}. Run a simulation export first.")
        return 1

    df = pd.read_csv(CSV)
    if len(df) < 2:
        print("CSV has too few samples to replay.")
        return 1

    stride = max(1, len(df) // 1200)
    frames = df.iloc[::stride].reset_index(drop=True)

    fig, ax = plt.subplots(figsize=(10, 6))
    ax.set_title("RocketStudio Trajectory Replay")
    ax.set_xlabel("Downrange distance (m)")
    ax.set_ylabel("Altitude (m)")
    ax.grid(True, alpha=0.35)
    ax.set_xlim(float(np.min(df["x"])) - 1000, float(np.max(df["x"])) + 1000)
    ax.set_ylim(0, max(1000.0, float(np.max(df["altitude"])) * 1.05))

    trail, = ax.plot([], [], color="#3aa3ff", linewidth=1.8)
    point, = ax.plot([], [], marker="o", color="#f2f5f8", markersize=6)
    time_text = ax.text(0.02, 0.95, "", transform=ax.transAxes)

    def update(frame_index: int):
        subset = frames.iloc[: frame_index + 1]
        trail.set_data(subset["x"], subset["altitude"])
        current = frames.iloc[frame_index]
        point.set_data([current["x"]], [current["altitude"]])
        time_text.set_text(f"t = {current['time']:.1f} s")
        return trail, point, time_text

    animation.FuncAnimation(fig, update, frames=len(frames), interval=20, blit=True)
    plt.show()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
