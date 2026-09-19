#!/usr/bin/env python3
"""Focused contracts for the sbrik Assets/ANM feature-rebase class."""

from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
INDEX = (ROOT / "src" / "AnmIdx.hpp").read_text(encoding="utf-8")
HEADER = (ROOT / "src" / "AnmManager.hpp").read_text(encoding="utf-8")
SOURCE = (ROOT / "src" / "AnmManager.cpp").read_text(encoding="utf-8")
PLAYER = (ROOT / "src" / "Player.cpp").read_text(encoding="utf-8")
BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"
FINAL = "022c533"


def require(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    print(f"PASS: {name}")


def git(*args: str) -> str:
    return subprocess.check_output(
        ["git", *args], cwd=ROOT, text=True, encoding="utf-8", errors="replace"
    ).replace("\r\n", "\n")


def hunk_count(path: str) -> int:
    diff = git("diff", "--unified=0", f"{BASE}..{FINAL}", "--", path)
    return len(re.findall(r"^@@ ", diff, flags=re.MULTILINE))


def define(name: str, source: str = INDEX) -> int:
    match = re.search(rf"^#define\s+{re.escape(name)}\s+(0x[0-9a-fA-F]+|\d+)",
                      source, flags=re.MULTILINE)
    if not match:
        raise AssertionError(f"missing define {name}")
    return int(match.group(1), 0)


def main() -> None:
    require("frozen AnmIdx.hpp has four hunks", hunk_count("src/th07/AnmIdx.hpp") == 4)
    require("frozen AnmManager.cpp has sixteen hunks",
            hunk_count("src/th07/AnmManager.cpp") == 16)
    require("frozen AnmManager.hpp has four hunks",
            hunk_count("src/th07/AnmManager.hpp") == 4)

    require("multiplayer player file slots avoid original menu children",
            define("ANM_FILE_PLAYER2") == 250 and define("ANM_FILE_PLAYER3") == 251)
    require("multiplayer face file slots reserve two entries each",
            define("ANM_FILE_FACE2") == 252 and define("ANM_FILE_FACE3") == 254)
    require("original multi-entry menu file ranges stay collision-free",
            define("ANM_FILE_TITLE") == 32 and
            define("ANM_FILE_RESULT") == 42 and
            define("ANM_FILE_MUSIC") == 46 and
            define("ANM_FILE_STAFF") == 49 and
            define("ANM_FILE_PLAYER2") > 49 and
            define("ANM_FILE_FACE2") + 1 < define("ANM_FILE_FACE3") and
            define("ANM_FILE_FACE3") + 1 < 256)
    require("player script blocks match upstream",
            define("ANM_OFFSET_PLAYER2") == 0x500 and
            define("ANM_OFFSET_PLAYER3") == 0xA00)
    require("face script blocks remain 0xa0 after player blocks",
            define("ANM_OFFSET_FACE2") - define("ANM_OFFSET_PLAYER2") == 0xA0 and
            define("ANM_OFFSET_FACE3") - define("ANM_OFFSET_PLAYER3") == 0xA0)

    require("portable sprite capacity matches final upstream",
            "constexpr i32 ANM_SPRITE_SLOT_COUNT = 2816;" in HEADER)
    require("portable file capacity matches final upstream",
            "constexpr i32 ANM_FILE_SLOT_COUNT = 256;" in HEADER)
    require("all sprite and script arrays share the capacity constant",
            "sprites[ANM_SPRITE_SLOT_COUNT]" in HEADER and
            "scripts[ANM_SPRITE_SLOT_COUNT]" in HEADER and
            "spriteIndices[ANM_SPRITE_SLOT_COUNT]" in HEADER)
    require("ANM file array uses the expanded capacity",
            "anmFiles[ANM_FILE_SLOT_COUNT]" in HEADER)
    require("constructor initializes every expanded sprite slot",
            "i < ANM_SPRITE_SLOT_COUNT" in SOURCE and
            "this->sprites[i].sourceFileIndex = -1" in SOURCE)
    require("load bounds use the shared capacities",
            "textureIdx >= ANM_FILE_SLOT_COUNT" in SOURCE and
            SOURCE.count(">= ANM_SPRITE_SLOT_COUNT") >= 2)
    require("release bounds use the shared file capacity",
            "(u32)anmIdx >= ANM_FILE_SLOT_COUNT" in SOURCE)

    require("each player loads its independent ANM file and offset",
            "playerAnmFile = ANM_FILE_PLAYER2" in PLAYER and
            "playerAnmFile = ANM_FILE_PLAYER3" in PLAYER and
            "playerAnmOffset = ANM_OFFSET_PLAYER2" in PLAYER and
            "playerAnmOffset = ANM_OFFSET_PLAYER3" in PLAYER)
    require("each player releases its own ANM file",
            "g_AnmManager->ReleaseAnm(playerAnmFile);" in PLAYER)
    require("portable renderer layout does not claim native byte size",
            "sizeof(AnmManager)" not in HEADER)
    require("Web transition cache stays presentation-only",
            "WebTransitionAnmCacheEntry" in SOURCE and
            "PreloadTransitionAnms" in SOURCE)


if __name__ == "__main__":
    main()
