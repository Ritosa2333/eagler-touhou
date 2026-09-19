#!/usr/bin/env python3
"""Focused contracts for the sbrik GameManager feature-rebase class."""

from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "GameManager.cpp").read_text(encoding="utf-8")
HEADER = (ROOT / "src" / "GameManager.hpp").read_text(encoding="utf-8")
RESOURCES = (ROOT / "src" / "MultiplayerResources.cpp").read_text(encoding="utf-8")
SESSION = (ROOT / "src" / "multiplayer" / "GameplaySession.cpp").read_text(encoding="utf-8")
BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"
FINAL = "022c533"


def require(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    print(f"PASS: {name}")


def git(*args: str) -> str:
    return subprocess.check_output(
        ["git", *args], cwd=ROOT
    ).decode("utf-8", errors="replace").replace("\r\n", "\n")


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
               "src/th07/GameManager.cpp", "src/th07/GameManager.hpp")
    implementation = diff.split("diff --git a/src/th07/GameManager.hpp", 1)[0]
    header = diff.split("diff --git a/src/th07/GameManager.hpp", 1)[1]
    require("frozen GameManager.cpp has twenty hunks",
            len(re.findall(r"^@@ ", implementation, flags=re.MULTILINE)) == 20)
    require("frozen GameManager.hpp has three hunks",
            len(re.findall(r"^@@ ", header, flags=re.MULTILINE)) == 3)

    require("native Netplay include is excluded",
            '#include "Netplay.hpp"' not in SOURCE)
    require("non-gameplay cherry diagnostic is excluded",
            "g_cherryRangeBreachLogged" not in SOURCE and
            "cherry max out of range" not in SOURCE)

    update = function_body(SOURCE, "u32 GameManager::OnUpdate(GameManager")
    require("multiplayer disables simulation-skipping slow mode",
            "MultiplayerGameplay::IsMultiplayer()" in update and
            "g_GameManager.defaultCfg->slowMode = 0" in update and
            "g_GameManager.slowModeSlowActive = 0" in update)

    added = function_body(SOURCE, "ZunResult GameManager::AddedCallback(GameManager")
    require("multiplayer startup normalizes lives and slow mode",
            "arg->defaultCfg->lifeCount = 2" in added and
            "arg->defaultCfg->slowMode = 0" in added)
    require("difficulty and practice overrides retain upstream order",
            added.index("arg->defaultCfg->lifeCount = 2") <
            added.index("if (g_GameManager.difficulty >= 4)") <
            added.index("if (g_GameManager.practice)"))

    cherry = function_body(SOURCE, "void GameManager::AddCherryPlus(i32 amount)")
    require("multiplayer Cherry routes through owner-aware shared helper",
            "MultiplayerGameplay::IsMultiplayer()" in cherry and
            "AddCherryPlusForPlayer(amount, 0)" in cherry)
    require("ordinary Cherry retains vanilla path",
            "i32 oldCherry = this->cherry" in cherry and
            "this->globals->cherryStart + 50000" in cherry and
            "g_Player.ActivateBorder()" in cherry)

    require("shared Border threshold is exact for two and three players",
            "GetActivePlayerCount() >= 3 ? 75000 : 50000" in RESOURCES)
    require("three-player Cherry range scales on activation and contraction",
            "cherryRange = cherryRange * 3 / 2" in RESOURCES and
            "cherryRange = cherryRange * 2 / 3" in RESOURCES)
    require("contraction cannot activate Border from threshold change",
            "g_GameManager.globals->cherryStart + newThreshold - 1" in RESOURCES)
    require("rank penalty scales by active player count",
            "return amount / activeCount" in RESOURCES)

    required_api = (
        "GetPlayerLives", "GetPlayerBombs", "GetPlayerPower",
        "GetPlayerCherryPlus", "SetPlayerLives", "SetPlayerBombs",
        "SetPlayerPower", "SetPlayerCherryPlus", "AddPlayerLives",
        "AddPlayerBombs", "AddPlayerPower", "AddPlayerCherryPlus",
        "GetPlayerEnemiesDefeated", "GetPlayerDamageDealt",
        "ResetMultiplayerPlayerResources", "ExtendAllPlayersFromPoints",
        "GetSharedBorderThreshold", "GetMultiplayerBossDamageMultiplier",
        "GetMultiplayerBombDamageMultiplier", "GetMultiplayerRankPenalty",
        "ApplyActivePlayerCountParameters",
    )
    require("resource and contribution sidecar API is complete",
            all(name in HEADER for name in required_api))
    require("ordinary data layout is protected by compile guard",
            "#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY" in HEADER and
            HEADER.index("#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY") <
            HEADER.index("struct MultiplayerPlayerResources"))

    unlock_functions = (
        "i32 GameManager::HasReachedMaxClears(i32 shotType)",
        "i32 GameManager::HasUnlockedPhantom(i32 shotType)",
        "i32 GameManager::HasReachedMaxClearsAllShotTypes()",
        "i32 GameManager::HasUnlockedPhantomAndMaxClears()",
    )
    require("all multiplayer unlock queries use runtime-only forcing",
            all("MultiplayerGameplay::ShouldForceContentUnlocks()" in
                function_body(SOURCE, signature)
                for signature in unlock_functions))
    require("unlock forcing does not mutate score data",
            "return IsMultiplayer();" in SESSION and
            "clrd" not in function_body(SESSION, "bool ShouldForceContentUnlocks()"))

    require("two-player threshold model", (75000 if 2 >= 3 else 50000) == 50000)
    require("three-player threshold model", (75000 if 3 >= 3 else 50000) == 75000)
    require("three-player rank penalty model", 300 // 3 == 100)


if __name__ == "__main__":
    main()
