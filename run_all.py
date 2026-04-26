from __future__ import annotations

import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parent
BUILD = ROOT / "build"
EXE = BUILD / "RocketStudio.exe"
CONFIG = ROOT / "data" / "default_config.json"
OUTPUT = ROOT / "data" / "output.csv"
SUMMARY = ROOT / "data" / "summary.json"


def run(command: list[str], cwd: Path) -> None:
    print("$", " ".join(command))
    subprocess.run(command, cwd=cwd, check=True)


def build() -> None:
    BUILD.mkdir(exist_ok=True)
    run(["cmake", "-G", "Ninja", "../cpp"], BUILD)
    run(["ninja"], BUILD)


def main() -> int:
    try:
        if not EXE.exists():
            build()

        run(
            [
                str(EXE),
                "--headless",
                "--config",
                str(CONFIG),
                "--output",
                str(OUTPUT),
                "--summary",
                str(SUMMARY),
            ],
            ROOT,
        )
        run([sys.executable, str(ROOT / "python_tools" / "plot_results.py")], ROOT)
        run([sys.executable, str(ROOT / "python_tools" / "analyze_results.py")], ROOT)
    except FileNotFoundError as exc:
        print(f"Missing tool: {exc.filename}")
        return 1
    except subprocess.CalledProcessError as exc:
        print(f"Command failed with exit code {exc.returncode}")
        return exc.returncode

    print(f"Files saved in: {ROOT / 'data'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
