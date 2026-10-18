#!/usr/bin/env python3
"""Focused contracts for the sbrik EclManager feature-rebase class."""

from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "EclManager.cpp").read_text(encoding="utf-8")
HEADER = (ROOT / "src" / "EclManager.hpp").read_text(encoding="utf-8")
SESSION_H = (ROOT / "src" / "multiplayer" / "GameplaySession.hpp").read_text(encoding="utf-8")
SESSION_C = (ROOT / "src" / "multiplayer" / "GameplaySession.cpp").read_text(encoding="utf-8")
ROLLBACK = (ROOT / "src" / "netplay" / "Th07RollbackState.cpp").read_text(encoding="utf-8")
SHELL = (ROOT / "resources" / "shell.html").read_text(encoding="utf-8")
BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"
FINAL = "022c533"


def require(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    print(f"PASS: {name}")


def git(*args: str) -> str:
    return subprocess.check_output(["git", *args], cwd=ROOT).decode(
        "utf-8", errors="replace").replace("\r\n", "\n")


def main() -> None:
    diff_cpp = git("diff", "--unified=0", f"{BASE}..{FINAL}", "--",
                   "src/th07/EclManager.cpp")
    diff_hpp = git("diff", "--unified=0", f"{BASE}..{FINAL}", "--",
                   "src/th07/EclManager.hpp")
    require("frozen EclManager.cpp has thirty hunks",
            len(re.findall(r"^@@ ", diff_cpp, flags=re.MULTILINE)) == 30)
    require("frozen EclManager.hpp has one hunk",
            len(re.findall(r"^@@ ", diff_hpp, flags=re.MULTILINE)) == 1)

    require("native Netplay shell and trace writers are excluded",
            '#include "Netplay.hpp"' not in SOURCE and
            "WriteTraceLine" not in SOURCE and "TraceBossEclSub" not in SOURCE)
    require("ordinary ECL targeting retains vanilla P1 fallback",
            "#define ECL_TARGET_PLAYER(enemy) (g_Player)" in SOURCE)
    require("multiplayer ECL targeting uses closest active logical player",
            "#define ECL_TARGET_PLAYER(enemy) (*GetClosestActivePlayer(&(enemy)->pos))" in SOURCE)
    require("all ten value target reads use the shared target resolver",
            SOURCE.count("ECL_TARGET_PLAYER(enemy).positionCenter") >= 9 and
            SOURCE.count("ECL_TARGET_PLAYER(enemy).AngleToPlayer") >= 3)
    require("all three writable player coordinates use the logical target",
            all(f"return &ECL_TARGET_PLAYER(enemy).positionCenter.{axis};" in SOURCE
                for axis in "xyz"))
    require("ECL movement laser and side decisions share logical targeting",
            "case ECL_MOVE_AT_PLAYER:" in SOURCE and
            "enemy->lasers[arg]->angle" in SOURCE and
            SOURCE.count("positionCenter.x < enemy->pos.x") == 2)
    require("ECL item drops use multiplayer-aware enemy drop spawning",
            "g_ItemManager.SpawnEnemyDrop(&enemy->pos" in SOURCE)
    require("late spell behavior observes every active player Bomb",
            "#define ECL_ANY_PLAYER_BOMBING() IsAnyActivePlayerBombing()" in SOURCE)

    require("Stage 4 character and difficulty table matches final upstream",
            "{127, 127, 128, 128}" in SOURCE and
            "{145, 145, 145, 145}" in SOURCE and
            "{138, 138, 138, 138}" in SOURCE)
    require("Stage 4 chain is host-authored and off by default",
            "bool stage4BossChain = false;" in SESSION_H and
            "return IsMultiplayer() && g_State.stage4BossChain;" in SESSION_C)
    require("Stage 4 chain no longer follows multiplayer unconditionally",
            "if (!MultiplayerGameplay::IsStage4BossChainEnabled()" in SOURCE and
            "if (!MultiplayerGameplay::IsMultiplayer() || !enemy" not in SOURCE)
    require("browser room option is boolean validated and defaults off",
            SHELL.count('"netplayStage4BossChain"') >= 1 and
            "netplayStage4BossChain: !!options.netplayStage4BossChain" in SHELL)
    require("Stage 4 queue is distinct-character stable-slot order",
            "for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)" in SOURCE and
            "g_stage4ChainQueue[queued++] = candidate" in SOURCE)
    require("Stage 4 phase restart validates card boss and sister",
            "enemy->bossId != 0" in SOURCE and
            "spellcardIdx != g_stage4ChainSpellIdx" in SOURCE and
            "!sister || !sister->active || difficulty < 0" in SOURCE)
    require("Stage 4 state is reset at EnemyManager lifecycle boundary",
            "void ResetStage4BossChain();" in HEADER and
            "ResetStage4BossChain();" in (ROOT / "src" / "EnemyManager.cpp").read_text(encoding="utf-8"))
    require("all seven Stage 4 fields are rollback-restored",
            all(name in ROLLBACK for name in (
                "g_stage4ChainQueue", "g_stage4ChainCount", "g_stage4ChainPos",
                "g_stage4ChainBossId", "g_stage4ChainCardActive",
                "g_stage4ChainPhaseLife", "g_stage4ChainSpellIdx")))
    require("ECL gameplay consumes no raw touch or presentation interpolation",
            "Touch::" not in SOURCE and "g_RenderAlpha" not in SOURCE and
            "GetPlayerPresentationOffset" not in SOURCE)


if __name__ == "__main__":
    main()
