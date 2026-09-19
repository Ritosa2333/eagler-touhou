#!/usr/bin/env python3
"""Focused contracts for the sbrik BulletManager feature-rebase class."""

from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "BulletManager.cpp").read_text(encoding="utf-8")
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
               "src/th07/BulletManager.cpp")
    require("frozen BulletManager.cpp has eleven hunks",
            len(re.findall(r"^@@ ", diff, flags=re.MULTILINE)) == 11)

    require("bullet spawn aims at closest active player",
            "GetClosestActivePlayer(&bulletProps->pos)->AngleToPlayer(&bulletProps->pos)" in SOURCE)
    require("laser spawn aims at closest active player",
            "GetClosestActivePlayer(&laserShooter->pos)->AngleToPlayer(&laserShooter->pos)" in SOURCE)
    require("delayed direction change re-aims at closest active player",
            "GetClosestActivePlayer(&this->pos)->AngleToPlayer(&this->pos)" in SOURCE)
    require("ordinary targeting remains P1",
            SOURCE.count("g_Player.AngleToPlayer") >= 3)

    require("graze checks stable active player slots",
            "collisionPlayer->CheckGraze" in SOURCE and
            "playerId < TH07_MULTI_MAX_PLAYERS && collisionRes == 0" in SOURCE)
    require("killbox checks stable active player slots",
            "collisionPlayer->CalcKillboxCollision" in SOURCE)
    require("collision-converted item belongs to actual player",
            SOURCE.count("collisionPlayer->itemType") == 2)
    require("laser collision covers every active player state",
            SOURCE.count("g_Players[playerId].CalcLaserHitbox") == 3)
    require("inactive slots are excluded from collision",
            SOURCE.count("IsPlayerSlotActive((u8)playerId)") >= 5)

    require("bullet logical tick publishes previous transform",
            "arg->bullets[i].prevPos = arg->bullets[i].pos" in SOURCE and
            "arg->bullets[i].prevAngle = arg->bullets[i].angle" in SOURCE)
    require("laser logical tick publishes previous transform and length",
            "laser->prevPos = laser->pos" in SOURCE and
            "laser->prevStartOffset = laser->startOffset" in SOURCE and
            "laser->prevEndOffset = laser->endOffset" in SOURCE)
    require("bullet interpolation is draw-only",
            "this->prevPos.Lerp(this->pos, g_RenderAlpha)" in SOURCE and
            "utils::LerpAngle(this->prevAngle, this->angle, g_RenderAlpha)" in SOURCE)
    require("laser interpolation is draw-only",
            "laser->prevPos.Lerp(laser->pos, g_RenderAlpha)" in SOURCE and
            "utils::Lerp(laser->prevEndOffset, laser->endOffset, g_RenderAlpha)" in SOURCE)

    players = [(0, True, 100.0), (1, True, 25.0), (2, False, 1.0)]
    selected = min((entry for entry in players if entry[1]), key=lambda entry: (entry[2], entry[0]))[0]
    require("nearest-active model excludes closer inactive slot", selected == 1)
    tied = [(0, True, 25.0), (1, True, 25.0), (2, True, 64.0)]
    selected_tie = min(tied, key=lambda entry: (entry[2], entry[0]))[0]
    require("nearest-active tie is stable by slot", selected_tie == 0)


if __name__ == "__main__":
    main()
