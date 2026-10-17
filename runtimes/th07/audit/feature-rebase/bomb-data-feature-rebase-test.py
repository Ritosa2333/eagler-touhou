#!/usr/bin/env python3
"""Focused contracts for the sbrik BombData feature-rebase class."""

from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "BombData.cpp").read_text(encoding="utf-8")
PLAYER = (ROOT / "src" / "Player.cpp").read_text(encoding="utf-8")
BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"
FINAL = "022c533"


def require(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    print(f"PASS: {name}")


def git(*args: str) -> str:
    return subprocess.check_output(["git", *args], cwd=ROOT).decode(
        "utf-8", errors="replace").replace("\r\n", "\n")


def function_body(source: str, signature: str) -> str:
    start = source.index(signature)
    brace = source.index("{", start)
    depth = 0
    for index in range(brace, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                return source[start:index + 1]
    raise AssertionError(signature)


def main() -> None:
    diff = git("diff", "--unified=0", f"{BASE}..{FINAL}", "--",
               "src/th07/BombData.cpp")
    require("frozen BombData.cpp has twenty-seven hunks",
            len(re.findall(r"^@@ ", diff, flags=re.MULTILINE)) == 27)

    resolver = function_body(SOURCE, "i32 ResolveBombAnmScript")
    require("multiplayer Bomb scripts use owner slot offsets",
            "GetPlayerAnmScript(player, script)" in resolver)
    require("ordinary Bomb scripts retain vanilla indices",
            "return script;" in resolver and
            "#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY" in resolver)
    effect_resolver = function_body(SOURCE, "i32 ResolveBombEffectSlot")
    require("multiplayer invulnerability effect uses owner fixed slot",
            "GetPlayerEffectSlot(player, p1Slot)" in effect_resolver)
    require("ordinary invulnerability effect retains P1 slot",
            "return p1Slot;" in effect_resolver)

    portraits = re.findall(r"ShowBombNamePortrait\(ResolveBombAnmScript\(player,\s*\d+\)", SOURCE)
    require("all twelve Bomb variants use owner portrait scripts",
            len(portraits) == 12)
    require("all fourteen Bomb VM initialization hunks use owner scripts",
            SOURCE.count("ResolveBombAnmScript(") - 1 - len(portraits) == 14)

    expected_calc = (
        "BombReimuACalc", "BombReimuACalcFocus",
        "BombReimuBCalc", "BombReimuBCalcFocus",
        "BombMarisaACalc", "BombMarisaACalcFocus",
        "BombMarisaBCalc", "BombMarisaBCalcFocus",
        "BombSakuyaACalc", "BombSakuyaACalcFocus",
        "BombSakuyaBCalc", "BombSakuyaBCalcFocus",
    )
    require("every character Shot and focus Bomb path is present",
            all(f"void BombData::{name}(Player *player)" in SOURCE
                for name in expected_calc))
    require("Bomb state is always owned by function player parameter",
            all("player->bombInfo" in
                function_body(SOURCE, f"void BombData::{name}(Player *player)")
                for name in expected_calc))

    draw_names = tuple(name.replace("Calc", "Draw") for name in expected_calc)
    require("every Bomb draw path receives the owner player",
            all(f"void BombData::{name}(Player *player)" in SOURCE
                for name in draw_names))
    draw_bodies = tuple(function_body(
        SOURCE, f"void BombData::{name}(Player *player)") for name in draw_names)
    require("Bomb presentation interpolates logical region positions",
            sum("g_RenderAlpha" in body for body in draw_bodies) >= 8 and
            SOURCE.count("prevBombRegionPositions.Lerp") >= 6)
    require("Bomb logical initialization publishes both position endpoints",
            SOURCE.count("prevBombRegionPositions =") >= 8)

    require("BombData does not sample host touch or presentation smoothing",
            "Touch::" not in SOURCE and
            "GetPlayerPresentationOffset" not in SOURCE)
    require("Bomb damage remains owner-specific and applies final 3P multiplier",
            "player->bombDamageBoxes" in SOURCE and
            "GetMultiplayerBombDamageMultiplier()" in PLAYER)
    require("Bomb clear behavior remains exact shared item removal",
            SOURCE.count("g_ItemManager.RemoveAllItems()") >= 8)


if __name__ == "__main__":
    main()
