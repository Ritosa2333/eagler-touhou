#!/usr/bin/env python3
"""Focused contracts for the sbrik Gui HUD feature-rebase class."""

from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "Gui.cpp").read_text(encoding="utf-8")
SESSION_H = (ROOT / "src" / "multiplayer" / "GameplaySession.hpp").read_text(encoding="utf-8")
SESSION_C = (ROOT / "src" / "multiplayer" / "GameplaySession.cpp").read_text(encoding="utf-8")
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
               "src/th07/Gui.cpp")
    require("frozen Gui.cpp has fifty-four hunks",
            len(re.findall(r"^@@ ", diff, flags=re.MULTILINE)) == 54)
    require("native Netplay Gui dependency is excluded",
            '#include "Netplay.hpp"' not in SOURCE)

    require("loadout labels use transport-neutral session loadouts",
            "MultiplayerGameplay::GetPlayerCharacter(playerId) * 2" in SOURCE and
            "MultiplayerGameplay::GetPlayerShot(playerId)" in SOURCE)

    bomb = function_body(SOURCE, "void Gui::ShowBombNamePortrait")
    require("P2 and P3 bomb portraits use independent face script blocks",
            "ANM_OFFSET_FACE2 - ANM_OFFSET_FACE" in bomb and
            "ANM_OFFSET_FACE3 - ANM_OFFSET_FACE" in bomb)
    require("unloaded bomb portrait has bounded P1 fallback",
            "sprite < 0" in bomb and "sourceFileIndex < 0" in bomb and
            "sprite = ANM_OFFSET_FACE + 1" in bomb)
    added = function_body(SOURCE, "ZunResult Gui::ActualAddedCallback()")
    require("secondary face ANMs load once at Gui registration",
            "ANM_FILE_FACE2" in added and "ANM_FILE_FACE3" in added and
            "GetPlayerFacePath(1)" in added and "GetPlayerFacePath(2)" in added)

    run_msg = function_body(SOURCE, "ZunResult GuiImpl::RunMsg()")
    require("dialogue breaks one shared Border through stable active slots",
            "playerId < TH07_MULTI_MAX_PLAYERS" in run_msg and
            "IsPlayerSlotActive(playerId)" in run_msg and
            "g_Players[playerId].BreakBorderNaturally()" in run_msg and
            "break;" in run_msg)
    require("ordinary dialogue retains P1 Border path",
            "g_Player.BreakBorderNaturally()" in run_msg)

    update = function_body(SOURCE, "void Gui::UpdateGui()")
    require("per-stage Cherry contribution diagnostics reset for all slots",
            "g_cherryMaxGrazeGrowth[playerId] = 0" in update and
            "g_cherryMaxBreakGrowth[playerId] = 0" in update)

    scene = function_body(SOURCE, "void Gui::DrawGameScene()")
    require("compact HUD is multiplayer-only",
            "MultiplayerGameplay::IsMultiplayer()" in scene and
            "const bool compactMultiplayerHud = false" in scene)
    require("pause and retry force static border redraw",
            "g_GameManager.isInPauseMenu || g_GameManager.isInRetryMenu" in scene)
    require("compact HUD uses exact upstream row geometry",
            "multiplayerHudBaseY = 76.0f" in scene and
            "multiplayerHudOffsetX = 8.0f" in scene and
            "resourceIconStep = 11.0f" in scene)
    require("three resource rows use logical per-player values",
            "GetPlayerLives((u8)playerId)" in scene and
            "GetPlayerBombs((u8)playerId)" in scene and
            "GetPlayerPower((u8)playerId)" in scene)
    require("temporarily absent slots hide resource labels and values",
            scene.count("MultiplayerGameplay::IsPlayerTemporarilyAbsent((u8)playerId)") >= 4)
    require("life and Bomb icon interpolation scale is synchronized and restored",
            "savedIconPrevScale" in scene and
            "savedBombIconPrevScale" in scene and
            scene.count("vm->prevScale = vm->scale") >= 2)
    require("label copies synchronize temporary interpolation scale",
            "playerLabelVm.prevScale = playerLabelVm.scale" in scene and
            "bombLabelVm.prevScale = bombLabelVm.scale" in scene and
            "powerLabelVm.prevScale = powerLabelVm.scale" in scene)
    require("Graze and Point use half-width clearing without logo erasure",
            "rowBackgroundVm.scale.x *= 0.5f" in scene and
            "ZunVec3(480.0f, 224.0f, 0.48f)" in scene and
            "ZunVec3(480.0f, 240.0f, 0.48f)" in scene)
    require("temporary row-background scale is interpolation-safe",
            "rowBackgroundVm.prevScale = rowBackgroundVm.scale" in scene)
    require("Graze and Point compact text uses and restores 0.70 scale",
            "savedGrazeScale" in scene and "savedPointScale" in scene and
            scene.count("Float2{0.70f, 0.70f}") >= 3)
    require("player rows show slot loadout contribution and absence state",
            '"P%d"' in scene and "GetMultiplayerHudLoadoutName" in scene and
            '"K:%u D:%u"' in scene and '"AWAY"' in scene)
    require("contribution display preference is local presentation state",
            "showContributionStats = true" in SESSION_H and
            "ShouldShowContributionStats" in SESSION_H and
            "return IsMultiplayer() && g_State.showContributionStats" in SESSION_C)
    require("power bars use one independent logical row per player",
            "const i32 playerPower = GetPlayerPower((u8)playerId)" in scene and
            "48.0f * playerId" in scene and "playerPower * 0.25f" in scene)
    require("portable renderer owns compact power quads",
            "gfxDevice->DrawPrimitiveUP" in scene and
            "D3DPT_TRIANGLESTRIP" not in scene and
            "d3dDevice" not in scene)
    require("HUD consumes no raw touch or smoothed simulation coordinate",
            "Touch::" not in scene and "GetPlayerPresentationOffset" not in scene)

    stage = function_body(SOURCE, "void Gui::DrawStageElements()")
    require("final sbrik does not draw one global Bomb portrait",
            "bombSpellcardPortrait.visible" not in stage and
            "bombSpellcardDecorLeft" not in stage and
            "bombSpellcardDecorRight" not in stage)
    require("remaining stage HUD keeps Eagler interpolation",
            "DrawInterp" in stage and "g_RenderAlpha" in stage)


if __name__ == "__main__":
    main()
