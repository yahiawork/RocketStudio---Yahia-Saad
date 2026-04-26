from __future__ import annotations

import json
from pathlib import Path

import pandas as pd


ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT / "data"
CSV = DATA / "output.csv"
SUMMARY = DATA / "summary.json"


def main() -> int:
    if not CSV.exists() or not SUMMARY.exists():
        print("Missing output.csv or summary.json. Run a simulation export first.")
        return 1

    df = pd.read_csv(CSV)
    summary = json.loads(SUMMARY.read_text(encoding="utf-8"))

    final = df.iloc[-1]
    print("RocketStudio Analysis")
    print(f"  Samples: {len(df)}")
    print(f"  Max altitude: {summary['maxAltitude']:.2f} m at {summary['maxAltitudeTime']:.2f} s")
    print(f"  Max velocity: {summary['maxVelocity']:.2f} m/s at {summary['maxVelocityTime']:.2f} s")
    print(f"  Max-Q: {summary['maxQ']:.2f} Pa at {summary['maxQTime']:.2f} s")
    print(f"  Total flight time: {summary['finalTime']:.2f} s")
    print(f"  Final altitude: {final['altitude']:.2f} m")
    print(f"  Final velocity: {final['speed']:.2f} m/s")
    print(f"  Escape velocity reached: {summary['escapeVelocityReached']}")
    print(f"  Stage events: {len(summary.get('stageEvents', []))}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
