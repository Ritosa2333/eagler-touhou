#!/usr/bin/env python3
"""Focused contracts for the sbrik main.cpp feature-rebase class."""
from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "main.cpp").read_text(encoding="utf-8")
SESSION = (ROOT / "src" / "multiplayer" / "GameplaySession.cpp").read_text(encoding="utf-8")
RESULT = (ROOT / "src" / "ResultScreen.cpp").read_text(encoding="utf-8")
BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"
FINAL = "022c533"


def require(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    print(f"PASS: {name}")


def git(*args: str) -> str:
    return subprocess.check_output(["git", *args], cwd=ROOT).decode(
        "utf-8", errors="replace"
    ).replace("\r\n", "\n")


def main() -> None:
    frozen = git(
        "diff", "--unified=0", f"{BASE}..{FINAL}", "--", "src/th07/main.cpp"
    )
    require(
        "frozen main.cpp has eleven hunks",
        len(re.findall(r"^@@ ", frozen, re.MULTILINE)) == 11,
    )
    require(
        "native Netplay and WinMain shell are excluded",
        '#include "Netplay.hpp"' not in SOURCE
        and "WINAPI WinMain" not in SOURCE,
    )
    require(
        "portable SDL callback lifecycle owns startup iteration and shutdown",
        all(
            marker in SOURCE
            for marker in ("SDL_AppInit", "SDL_AppIterate", "SDL_AppEvent", "SDL_AppQuit")
        ),
    )
    require(
        "browser LAN launch is an explicit room mode",
        "netplayMode === 'lan'" in SOURCE and "g_NetplayLanProduction" in SOURCE,
    )
    seed_index = SOURCE.index("g_Rng.seed = (u16)requestedSeed")
    configure_index = SOURCE.index("MultiplayerGameplay::Configure(gameplaySession)")
    dispatch_index = SOURCE.index(
        "g_GameManager.currentStage = requestedDifficulty", configure_index
    )
    require(
        "room RNG seed and gameplay session are installed before stage dispatch",
        seed_index < configure_index < dispatch_index
        and "g_Rng.generationCount = 0" in SOURCE[seed_index:configure_index]
        and "requestedDifficulty < DIFF_EXTRA"
        in SOURCE[dispatch_index : dispatch_index + 180],
    )
    require(
        "room descriptor supplies stable slot count and local ownership",
        "netplayPlayerCount" in SOURCE
        and "netplayPlayer ?? 0" in SOURCE
        and "gameplaySession.localPlayer" in SOURCE,
    )
    require(
        "room descriptor supplies every active slot loadout",
        "netplayLoadouts" in SOURCE
        and "gameplaySession.players[playerId].character" in SOURCE
        and "gameplaySession.players[playerId].shot" in SOURCE,
    )
    require(
        "host-authored Stage 4 behavior is installed before configuration",
        "gameplaySession.stage4BossChain = EaglerOptions::NetplayStage4BossChain()"
        in SOURCE,
    )
    require(
        "session validation rejects invalid gameplay descriptors without a native protocol gate",
        "if (!MultiplayerGameplay::Configure(gameplaySession))" in SOURCE
        and "return false" in SESSION,
    )
    require(
        "multiplayer score and replay persistence are isolated in their owning class",
        "ShouldSkipPersistentResultWrite" in RESULT
        and "ShouldSkipReplaySavePrompt" in RESULT
        and "MultiplayerGameplay::IsMultiplayer()" in RESULT,
    )
    require(
        "display audio touch and exit lifecycle stay endpoint-local",
        "GameWindow::ToggleFullscreen()" in SOURCE
        and "Touch::CancelTouches()" in SOURCE
        and "SuspendAudioForInactiveWindow()" in SOURCE
        and "EaglerTouhouGameExited" in SOURCE,
    )
    require(
        "entry audit introduces no native socket thread low-latency or compatibility hash shell",
        not any(
            token in SOURCE
            for token in (
                "WinSock",
                "LowLatency",
                "CreateThread",
                "AllowsMultipleInstances",
                "GetStatusText",
                "compatibility hash",
            )
        ),
    )
    require(
        "existing executable checksum call remains untouched rather than expanded",
        SOURCE.count("GameWindow::ChecksumExecutable();") == 1
        and "ChecksumExecutable" not in frozen,
    )


if __name__ == "__main__":
    main()
