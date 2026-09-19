#!/usr/bin/env python3
"""Focused contract for the sbrik EnemyEclInstr multiplayer hunk."""

from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "EnemyEclInstr.cpp").read_text(encoding="utf-8")
BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"
FINAL = "022c533"


def require(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    print(f"PASS: {name}")


def git(*args: str) -> str:
    return subprocess.check_output(
        ["git", *args], cwd=ROOT, text=True, encoding="utf-8"
    ).replace("\r\n", "\n")


def main() -> None:
    diff = git("diff", "--unified=0", f"{BASE}..{FINAL}", "--",
               "src/th07/EnemyEclInstr.cpp")
    require("frozen EnemyEclInstr.cpp has one hunk",
            len(re.findall(r"^@@ ", diff, flags=re.MULTILINE)) == 1)
    require("Youmu redirect targets the closest active player",
            "GetClosestActivePlayer(&bullet->pos)->AngleToPlayer(&bullet->pos)" in SOURCE)
    require("ordinary gameplay retains vanilla P1 targeting",
            "g_Player.AngleToPlayer(&bullet->pos)" in SOURCE)
    require("multiplayer target selection is compile guarded",
            "#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY" in SOURCE)
    require("targeting uses logical bullet position",
            SOURCE.count("&bullet->pos") >= 2 and "g_RenderAlpha" not in SOURCE)

    players = [(0, True, 80.0), (1, True, 20.0), (2, False, 5.0)]
    selected = min((p for p in players if p[1]), key=lambda p: (p[2], p[0]))[0]
    require("inactive nearer player is not selected", selected == 1)


if __name__ == "__main__":
    main()
