#!/usr/bin/env python3
"""Focused contracts for the sbrik EnemyManager feature-rebase class."""
from __future__ import annotations
import pathlib, re, subprocess

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "EnemyManager.cpp").read_text(encoding="utf-8")
MAIN = (ROOT / "src" / "main.cpp").read_text(encoding="utf-8")
BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"
FINAL = "022c533"

def require(name: str, condition: bool) -> None:
    if not condition: raise AssertionError(name)
    print(f"PASS: {name}")

def git(*args: str) -> str:
    return subprocess.check_output(["git", *args], cwd=ROOT).decode("utf-8", errors="replace").replace("\r\n", "\n")

def function_body(source: str, signature: str) -> str:
    start = source.index(signature); brace = source.index("{", start); depth = 0
    for index in range(brace, len(source)):
        if source[index] == "{": depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0: return source[start:index + 1]
    raise AssertionError(signature)

def main() -> None:
    diff = git("diff", "--unified=0", f"{BASE}..{FINAL}", "--", "src/th07/EnemyManager.cpp")
    require("frozen EnemyManager.cpp has thirty-three hunks", len(re.findall(r"^@@ ", diff, re.M)) == 33)
    require("native Netplay shell is excluded", '#include "Netplay.hpp"' not in SOURCE)
    require("timer callback cooperates with Stage 4 chain", "if (Stage4ChainRestartPhase(this, 1))" in SOURCE)
    require("enemy projectiles graze and hit all active slots", SOURCE.count("IsPlayerSlotActive(playerId)") >= 5 and "bool hitPlayer = false;" in SOURCE)
    require("damage is collected independently for all three slots", "playerDamage[TH07_MULTI_MAX_PLAYERS]" in SOURCE and "playerCollision[TH07_MULTI_MAX_PLAYERS]" in SOURCE)
    require("damage and Cherry owner uses deterministic lower-slot tie", "playerDamage[playerId] > playerDamage[damageOwnerId]" in SOURCE)
    require("owner focus Bomb and loadout drive Cherry behavior", "Player &damageOwner = g_Players[damageOwnerId]" in SOURCE and "GetPlayerShot((u8)damageOwnerId)" in SOURCE)
    require("Cherry contribution is attributed to damage owner", "AddCherryPlusForPlayer(cherryGain, (u8)damageOwnerId)" in SOURCE)
    require("Boss damage uses active-count scaling outside chained card", "GetMultiplayerBossDamageMultiplier()" in SOURCE and "!IsStage4ChainedCardActive()" in SOURCE)
    require("damage contribution preserves proportional split and remainder", "AddPlayerDamageDealt(playerId" in SOURCE and "damage - damageAttributed" in SOURCE)
    require("every active slot receives independent targeting state", "targetingPlayer = &g_Players[playerId]" in SOURCE and "GetPlayerCharacter(playerId) == CHAR_SAKUYA" in SOURCE)
    require("defeat contribution belongs to deterministic damage owner", "AddPlayerEnemiesDefeated((u8)damageOwnerId, 1)" in SOURCE)
    require("both enemy drop paths use multiplayer-aware distribution", SOURCE.count("SpawnEnemyDrop(") >= 2)
    require("Stage 4 state resets at stage EnemyManager registration", "ResetStage4BossChain();" in SOURCE)
    seed_index = MAIN.index("g_Rng.seed = (u16)requestedSeed;")
    configure_index = MAIN.index("MultiplayerGameplay::Configure(gameplaySession)")
    stage_index = MAIN.index("g_GameManager.currentStage = requestedDifficulty", configure_index)
    require("Eagler seeds RNG from room seed before gameplay registration",
            seed_index < configure_index < stage_index and
            "requestedDifficulty < DIFF_EXTRA" in MAIN[stage_index:stage_index + 180])
    update = function_body(SOURCE, "u32 EnemyManager::OnUpdate")
    require("Enemy gameplay consumes no raw touch or draw interpolation", "Touch::" not in update and "g_RenderAlpha" not in update and "GetPlayerPresentationOffset" not in update)

if __name__ == "__main__": main()
