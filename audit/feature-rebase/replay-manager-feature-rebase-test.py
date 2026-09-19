#!/usr/bin/env python3
"""Focused contracts for the sbrik ReplayManager feature-rebase class."""

from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "ReplayManager.cpp").read_text(encoding="utf-8")
EXTENSION = (ROOT / "src" / "ReplayExtension.cpp").read_text(encoding="utf-8")
PLAYER = (ROOT / "src" / "Player.cpp").read_text(encoding="utf-8")
MAIN_MENU = (ROOT / "src" / "MainMenu.cpp").read_text(encoding="utf-8")
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
               "src/th07/ReplayManager.cpp")
    require("frozen ReplayManager.cpp has eight hunks",
            len(re.findall(r"^@@ ", diff, flags=re.MULTILINE)) == 8)
    require("native Netplay replay shell is excluded",
            '#include "Netplay.hpp"' not in SOURCE and "Netplay::NoSave" not in SOURCE)

    update = function_body(SOURCE, "u32 ReplayManager::OnUpdate(ReplayManager")
    require("netplay does not advance logical edges twice",
            "if (!Netplay::Input::PlayerButtonOverridesActive())" in update)
    require("non-netplay multiplayer advances all previous lanes",
            "g_LastFrameGameInputs[playerId] = g_CurFrameGameInputs[playerId]" in update)
    require("non-netplay fallback keeps only physical P1",
            "g_CurFrameGameInputs[0] = g_CurFrameRawInput" in update and
            "g_CurFrameGameInputs[playerId] = 0" in update)
    require("speculative pass does not append replay frames",
            "if (Netplay::SideEffects::IsSpeculative())" in update)
    require("committed multiplayer replay records synchronized FrameInput lanes",
            "ReplayExtension::RecordMultiplayerFrame" in update and
            "Netplay::Input::PlayerInputOverride" in update)

    demo = function_body(SOURCE, "u32 ReplayManager::OnUpdateDemoHighPrio")
    require("vanilla replay playback clears P2 and P3",
            "for (i32 playerId = 1; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)" in demo and
            "g_CurFrameGameInputs[playerId] = 0" in demo)
    require("EAGX playback frame is applied before vanilla input",
            demo.index("ReplayExtension::SetPlaybackFrame") <
            demo.index("g_CurFrameGameInput = arg->replayInputs->frameNum"))
    require("multiplayer replay restores every synchronized player lane",
            "ReplayExtension::GetMultiplayerPlaybackFrame" in demo and
            "Netplay::Input::SetPlayerInputOverrides" in demo)
    player_register = function_body(PLAYER, "ZunResult Player::RegisterChain")
    require("multiplayer replay registers every active Player slot",
            "if (MultiplayerGameplay::IsMultiplayer())" in player_register and
            "!g_GameManager.replay" not in player_register and
            "RegisterOnePlayer(&g_Players[playerId], playerId)" in player_register)
    require("Replay menu restores multiplayer session metadata before gameplay starts",
            "ReplayExtension::GetMultiplayerPlaybackConfig" in MAIN_MENU and
            "MultiplayerGameplay::Configure(session)" in MAIN_MENU and
            "session.localPlayer = replayConfig.localPlayer" in MAIN_MENU and
            "session.stage4BossChain = replayConfig.stage4BossChain" in MAIN_MENU and
            "session.showContributionStats = replayConfig.showContributionStats" in MAIN_MENU and
            "session.showStagePlayerNames = replayConfig.showStagePlayerNames" in MAIN_MENU and
            "session.players[playerId].character = replayConfig.characters[playerId]" in MAIN_MENU and
            "session.players[playerId].shot = replayConfig.shots[playerId]" in MAIN_MENU)
    added = function_body(SOURCE, "ZunResult ReplayManager::AddedCallback(ReplayManager")
    require("multiplayer replay records deterministic Stage 4 chain and local presentation flags",
            "config.stage4BossChain = MultiplayerGameplay::IsStage4BossChainEnabled()" in added and
            "config.showContributionStats = MultiplayerGameplay::ShouldShowContributionStats()" in added and
            "config.showStagePlayerNames = MultiplayerGameplay::ShouldShowStagePlayerNames()" in added)
    require("multiplayer replay records each player's stage-start resources",
            "ReplayExtension::MultiplayerPlayerResourceSnapshot resources" in added and
            "resources[playerId].lives = GetPlayerLives(playerId)" in added and
            "resources[playerId].bombs = GetPlayerBombs(playerId)" in added and
            "resources[playerId].power = GetPlayerPower(playerId)" in added and
            "ReplayExtension::CaptureMultiplayerStageResources" in added)
    require("multiplayer replay records cumulative contribution HUD state at each stage start",
            "ReplayExtension::MultiplayerContributionSnapshot contributions" in added and
            "contributions[playerId].enemiesDefeated = GetPlayerEnemiesDefeated(playerId)" in added and
            "contributions[playerId].damageDealt = GetPlayerDamageDealt(playerId)" in added and
            "ReplayExtension::CaptureMultiplayerStageContributions" in added)
    added_demo = function_body(SOURCE, "ZunResult ReplayManager::AddedCallbackDemo")
    require("multiplayer replay restores stage resources after vanilla P1 state",
            "ReplayExtension::GetMultiplayerPlaybackStageResources" in added_demo and
            added_demo.index("g_GameManager.SetCurrentPower(replayData->currentPower)") <
            added_demo.index("ReplayExtension::GetMultiplayerPlaybackStageResources") and
            "SetPlayerLives(playerId, resources.lives)" in added_demo and
            "SetPlayerBombs(playerId, resources.bombs)" in added_demo and
            "SetPlayerPower(playerId, resources.power)" in added_demo)
    require("direct later-stage replay restores cumulative contribution HUD state",
            "ReplayExtension::GetMultiplayerPlaybackStageContributions" in added_demo and
            "g_MultiplayerContributionStats[playerId].enemiesDefeated" in added_demo and
            "g_MultiplayerContributionStats[playerId].damageDealt" in added_demo)

    require("replay Border cap uses shared multiplayer threshold",
            "const i32 borderThreshold" in SOURCE and
            "GetSharedBorderThreshold();" in SOURCE and
            "cherryStart + borderThreshold" in SOURCE)
    register = function_body(SOURCE, "ZunResult ReplayManager::RegisterChain")
    require("replay registration clears every player lane",
            "playerId < TH07_MULTI_MAX_PLAYERS" in register and
            "g_LastFrameGameInputs[playerId] = 0" in register and
            "g_CurFrameGameInputs[playerId] = 0" in register)

    save = function_body(SOURCE, "void ReplayManager::SaveReplay(")
    save2 = function_body(SOURCE, "void ReplayManager::SaveReplay2(")
    require("multiplayer save is no longer blocked by the vanilla single-lane guard",
            "MultiplayerGameplay::IsMultiplayer()" not in save and
            "MultiplayerGameplay::IsMultiplayer()" not in save2)
    require("ordinary save retains EAGX recording append",
            "ReplayExtension::AppendRecording" in save)
    require("ordinary rewrite retains EAGX playback append",
            "ReplayExtension::AppendPlayback" in save2)
    require("EAGX touch events remain per frame",
            "ReplayExtension::GetPlaybackTouchEvents" in SOURCE and
            "Touch::ApplyReplayTouchEvent" in SOURCE)
    require("EAGX trailer remains optional for vanilla replay",
            'std::memcmp(bytes + size - 4, "EAGX", 4)' in EXTENSION and
            "return false;" in EXTENSION)
    require("EAGX v1 multiplayer type carries metadata and three synchronized lanes",
            "FLAG_MULTIPLAYER_INPUT" in EXTENSION and
            "MultiplayerInputSample" in EXTENSION and
            "MP_PLAYER_COUNT_OFFSET" in EXTENSION and
            "MP_LOCAL_PLAYER_OFFSET" in EXTENSION and
            "MP_SESSION_FLAGS_OFFSET" in EXTENSION and
            "MP_SESSION_STAGE4_BOSS_CHAIN" in EXTENSION and
            "GetMultiplayerPlaybackConfig" in EXTENSION)
    require("EAGX v1 optionally appends per-stage multiplayer resources",
            "FLAG_MULTIPLAYER_STAGE_RESOURCES = 64" in EXTENSION and
            "MP_STAGE_RESOURCE_BYTES" in EXTENSION and
            "GetMultiplayerPlaybackStageResources" in EXTENSION and
            "legacyMultiplayerV1Accepted" in EXTENSION)
    require("EAGX v1 optionally appends per-stage multiplayer contribution HUD snapshots",
            "FLAG_MULTIPLAYER_STAGE_CONTRIBUTIONS = 128" in EXTENSION and
            "MP_STAGE_CONTRIBUTION_BYTES" in EXTENSION and
            "GetMultiplayerPlaybackStageContributions" in EXTENSION and
            "preContributionV1Accepted" in EXTENSION)
    require("Power-transfer tap gesture is reset at stage registration",
            "g_powerGiveTaps[playerId] = 0" in PLAYER and
            "g_powerGiveWindow[playerId] = 0" in PLAYER)


if __name__ == "__main__":
    main()
