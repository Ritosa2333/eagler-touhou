#!/usr/bin/env python3
"""Focused contracts for the sbrik Effects feature-rebase class."""

from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "EffectManager.cpp").read_text(encoding="utf-8")
HEADER = (ROOT / "src" / "EffectManager.hpp").read_text(encoding="utf-8")
ROLLBACK = (ROOT / "src" / "netplay" / "Th07RollbackState.cpp").read_text(encoding="utf-8")
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


def hunk_count(path: str) -> int:
    diff = git("diff", "--unified=0", f"{BASE}..{FINAL}", "--", path)
    return len(re.findall(r"^@@ ", diff, flags=re.MULTILINE))


def main() -> None:
    require("frozen EffectManager.cpp has fourteen hunks",
            hunk_count("src/th07/EffectManager.cpp") == 14)
    require("frozen EffectManager.hpp has two hunks",
            hunk_count("src/th07/EffectManager.hpp") == 2)

    require("effect storage has thirteen fixed player slots and one sentinel",
            "Effect effects[414];" in HEADER)
    require("particle allocation failure returns the sentinel",
            SOURCE.count("return i >= 400 ? &this->effects[413] : effect;") == 2)
    require("update processes every live slot but not the sentinel",
            "for (i = 0; i < 413; i++, effect++)" in SOURCE)
    require("attached effects find all three stable players",
            "playerId < TH07_MULTI_MAX_PLAYERS" in SOURCE and
            "player->effect == effect" in SOURCE and
            "player->focusEffect == effect" in SOURCE and
            "player->borderEffect == effect" in SOURCE)
    require("attached effect simulation follows logical player position",
            "effect->pos1 = player->positionCenter;" in SOURCE)
    require("ordinary gameplay retains vanilla P1 attachment",
            "effect->pos1 = g_Player.positionCenter;" in SOURCE)
    require("native focus diagnostic is excluded",
            "focus effect verified slot" not in SOURCE and "focusLogged" not in SOURCE)

    require("effect interpolation publishes previous position",
            "effect->prevPos = effect->pos1;" in SOURCE and
            "effect->vm.UpdatePrev();" in SOURCE)
    require("effect interpolation occurs only in draw",
            "active[i]->prevPos.Lerp(active[i]->pos1, g_RenderAlpha)" in SOURCE)
    require("attached effects receive the same presentation offset",
            "active[i]->vm.pos += GetPlayerPresentationOffset(playerId);" in SOURCE)
    require("player-attached overlap alpha is draw-only and restored",
            "GetPlayerOverlapAlpha(&g_Players[playerId])" in SOURCE and
            "active[i]->vm.color.color = originalColor;" in SOURCE)
    require("draw sorting keeps all 413 live effects addressable",
            "Effect *active[413];" in SOURCE)

    require("rollback journal visits all live effect slots",
            "for (int i = 0; i < 413; ++i)" in ROLLBACK and
            "TouchEffect(&g_EffectManager.effects[i])" in ROLLBACK)
    require("portable layout does not claim native byte size",
            "sizeof(EffectManager)" not in HEADER)


if __name__ == "__main__":
    main()
