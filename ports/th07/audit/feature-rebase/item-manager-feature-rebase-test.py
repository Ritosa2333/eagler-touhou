#!/usr/bin/env python3
"""Focused source and deterministic-behavior checks for the sbrik Items work class."""

from __future__ import annotations

import math
import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "ItemManager.cpp").read_text(encoding="utf-8")
HEADER = (ROOT / "src" / "ItemManager.hpp").read_text(encoding="utf-8")
UPSTREAM_BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"
UPSTREAM_FINAL = "022c533"


def require(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    print(f"PASS: {name}")


def git(*args: str) -> str:
    return subprocess.check_output(
        ["git", *args], cwd=ROOT, text=True, encoding="utf-8"
    )


def hunk_count(path: str) -> int:
    diff = git(
        "diff", "--unified=0", f"{UPSTREAM_BASE}..{UPSTREAM_FINAL}", "--", path
    )
    return len(re.findall(r"^@@ ", diff, flags=re.MULTILINE))


def transfer_state(player_id: int) -> int:
    return {0: 4, 1: 3, 2: 5}.get(player_id, 0)


def separated_x(origin_x: float, ordinal: int, count: int, width: float) -> float:
    half_span = 16.0 * (count - 1)
    center = min(max(origin_x, half_span), width - half_span)
    return center - half_span + ordinal * 32.0


def round_robin_target(active_ids: list[int], item_index: int) -> int:
    return active_ids[item_index % len(active_ids)]


def main() -> None:
    require("frozen upstream ItemManager.cpp has 49 hunks", hunk_count("src/th07/ItemManager.cpp") == 49)
    require("frozen upstream ItemManager.hpp has 2 hunks", hunk_count("src/th07/ItemManager.hpp") == 2)

    required_source = {
        "three transfer states": "return MultiplayerGameplay::IsMultiplayer() && state >= 3 && state <= 5;",
        "20-frame collision lock": "IsMultiplayerTransferState(item->state) && item->timer < 20",
        "inactive fixed target is released": "item->autoCollect = 0;\n            item->state = 0;",
        "shared Border can redistribute fixed items": "HasFixedItemTarget(item) && !IsSharedBorderActive()",
        "target-specific small power": "AddItemPlayerPower(targetPlayer, 1);",
        "target-specific large power": "AddItemPlayerPower(targetPlayer, 8);",
        "target-specific Bomb": "AddItemPlayerBombs(targetPlayer, 1);",
        "target-specific life": "ExtendFromLifeItem(targetPlayer);",
        "shared Cherry owner": "AddSharedCherryPlus(itemScore, targetPlayer);",
        "enemy-only duplicate drop path": "Item *ItemManager::SpawnEnemyDrop",
        "spawn publishes both interpolation endpoints": "item->prevPosition = item->currentPosition = *heading;",
        "logical tick publishes the prior item position": "item->prevPosition = item->currentPosition;",
        "draw interpolation is presentation-only": "item->prevPosition.Lerp(item->currentPosition, g_RenderAlpha)",
        "pickup targeting uses synchronized logical player position": "GetClosestActivePlayer(&item->currentPosition)",
    }
    for name, fragment in required_source.items():
        require(name, fragment in SOURCE)

    require("transfer API is declared", "GetLifeTransferSpawnState" in HEADER)
    require("native Netplay shell is not included", '#include "Netplay.hpp"' not in SOURCE)
    require("upstream native diagnostics are not called", "ReportLifeTransferTestResult" not in SOURCE)
    raw_touch_tokens = ("touchused", "touchbomb", "directtouch", "rawtouch", "touchposition")
    require(
        "Items does not read raw host touch state",
        not any(token in SOURCE.lower() for token in raw_touch_tokens),
    )

    require("P1/P2/P3 transfer state mapping", [transfer_state(i) for i in range(4)] == [4, 3, 5, 0])
    require("two-player drops are symmetric", [separated_x(100.0, i, 2, 384.0) for i in range(2)] == [84.0, 116.0])
    require("three-player drops are symmetric", [separated_x(100.0, i, 3, 384.0) for i in range(3)] == [68.0, 100.0, 132.0])
    require("edge drops stay inside the playfield", [separated_x(0.0, i, 3, 384.0) for i in range(3)] == [0.0, 32.0, 64.0])
    require("active-slot round robin is deterministic", [round_robin_target([0, 2], i) for i in range(6)] == [0, 2, 0, 2, 0, 2])

    samples = [1.0 - math.pow(1.0 - frame / 20.0, 1.5) for frame in range(20)]
    require("transfer rise easing is monotonic", all(a < b for a, b in zip(samples, samples[1:])))
    require("transfer rise starts at the donor", samples[0] == 0.0)
    require("transfer rise remains collision-locked through frame 19", samples[-1] < 1.0)


if __name__ == "__main__":
    main()
