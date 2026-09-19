#!/usr/bin/env python3
"""Focused contracts for the sbrik ResultScreen feature-rebase class."""
from __future__ import annotations
import pathlib, re, subprocess

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "ResultScreen.cpp").read_text(encoding="utf-8")
BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"; FINAL = "022c533"

def require(name, ok):
    if not ok: raise AssertionError(name)
    print(f"PASS: {name}")
def git(*args):
    return subprocess.check_output(["git", *args], cwd=ROOT).decode("utf-8", errors="replace").replace("\r\n", "\n")

def function_body(source, signature):
    start = source.index(signature); brace = source.index("{", start); depth = 0
    for index in range(brace, len(source)):
        if source[index] == "{": depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0: return source[start:index + 1]
    raise AssertionError(signature)

def main():
    diff = git("diff", "--unified=0", f"{BASE}..{FINAL}", "--", "src/th07/ResultScreen.cpp")
    require("frozen ResultScreen.cpp has twenty-eight hunks", len(re.findall(r"^@@ ", diff, re.M)) == 28)
    require("native Netplay shell is excluded", '#include "Netplay.hpp"' not in SOURCE and "Netplay::" not in SOURCE)
    require("multiplayer result writes do not persist score.dat", "ShouldSkipPersistentResultWrite" in SOURCE and "return MultiplayerGameplay::IsMultiplayer();" in SOURCE)
    require("ordinary result path retains persistent score writes", "#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY" in SOURCE and "return false;" in SOURCE and "FileSystem::WriteDataToFile(\"score.dat\"" in SOURCE)
    require("multiplayer result now keeps replay save prompt", "ShouldSkipReplaySavePrompt" in SOURCE and "return false;" in SOURCE and "resultScreenState = ShouldSkipReplaySavePrompt() ? 18 : 11" in SOURCE)
    require("result-name prompt is available to multiplayer replay", "if (!ShouldSkipReplaySavePrompt())" in SOURCE and "strcpy(arg->replayName" in SOURCE)
    require("result screen keeps local score parsing for display", "OpenScore(FileSystem::GetPrefPath(\"score.dat\")" in SOURCE and "ParseCatk" in SOURCE and "ParsePscr" in SOURCE)
    require(
        "Phantasm final-stats difficulty tables cover all six enum values",
        "g_DifficultyWeightsList[] = {-30.0f, -10.0f, 20.0f, 30.0f, 30.0f, 30.0f}" in SOURCE
        and "g_DifficultySpellcardWeightsList[] = {1.0f, 1.5f, 1.5f, 2.0f, 2.5f, 2.5f}" in SOURCE,
    )
    draw = function_body(SOURCE, "u32 ResultScreen::OnDraw")
    require("result drawing has no raw touch or presentation-offset dependency", "Touch::" not in draw and "GetPlayerPresentationOffset" not in draw)
    require("result path does not add hash or compatibility behavior", "HashState" not in diff and "Compatibility" not in diff and "checksum" not in diff.lower())

if __name__ == "__main__": main()
