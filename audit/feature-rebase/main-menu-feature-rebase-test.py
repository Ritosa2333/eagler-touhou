#!/usr/bin/env python3
"""Focused contracts for the sbrik MainMenu feature-rebase class."""
from __future__ import annotations
import pathlib, re, subprocess

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "MainMenu.cpp").read_text(encoding="utf-8")
SESSION = (ROOT / "src" / "multiplayer" / "GameplaySession.cpp").read_text(encoding="utf-8")
BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"; FINAL = "022c533"

def require(name, ok):
    if not ok: raise AssertionError(name)
    print(f"PASS: {name}")
def git(*args):
    return subprocess.check_output(["git", *args], cwd=ROOT).decode("utf-8", errors="replace").replace("\r\n", "\n")

def main():
    diff = git("diff", "--unified=0", f"{BASE}..{FINAL}", "--", "src/th07/MainMenu.cpp")
    require("frozen MainMenu.cpp has seventeen hunks", len(re.findall(r"^@@ ", diff, re.M)) == 17)
    require("native Netplay include and native quick-start APIs are excluded", '#include "Netplay.hpp"' not in SOURCE and "Netplay::" not in SOURCE)
    require("portable start helper centralizes stage launch cleanup", "static u32 StartSelectedGame(i32 stageBeforeIncrement)" in SOURCE and "g_SoundPlayer.ProcessQueues()" in SOURCE)
    require("normal selected-shot launch preserves vanilla stage mapping", "g_GameManager.difficulty < DIFF_EXTRA" in SOURCE and "StartSelectedGame(" in SOURCE)
    require("multiplayer practice forces runtime content availability", SOURCE.count("ShouldForceContentUnlocks()") >= 2 and "local_8 = 99" in SOURCE and "local_10 = 99" in SOURCE)
    require("runtime unlock decision never writes save data", "return IsMultiplayer();" in SESSION and "OpenFile" not in SESSION)
    require("multiplayer skips endpoint-local options entry", "IsMultiplayer() && this->cursor == 6" in SOURCE and "if (MultiplayerGameplay::IsMultiplayer())" in SOURCE)
    require("ordinary options and unlock paths remain outside multiplayer guards", "#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY" in SOURCE and "#endif" in SOURCE)
    require("menu cursor still wraps and plays movement feedback", "this->cursor--" in SOURCE and "this->cursor++" in SOURCE and "SOUND_MOVE_MENU" in SOURCE)
    require("menu draws through previous/current interpolation endpoints", "prevPos.Lerp(local_c->pos, g_RenderAlpha)" in SOURCE)
    require("menu never reads raw touch or presentation offsets for gameplay", "Touch::" not in SOURCE and "GetPlayerPresentationOffset" not in SOURCE)

if __name__ == "__main__": main()
