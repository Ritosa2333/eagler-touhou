from pathlib import Path


SOURCE = Path("src/ItemManager.cpp").read_text(encoding="utf-8")
HEADER = Path("src/ItemManager.hpp").read_text(encoding="utf-8")
ENEMY = Path("src/EnemyManager.cpp").read_text(encoding="utf-8")
ECL = Path("src/EclManager.cpp").read_text(encoding="utf-8")
PLAYER = Path("src/Player.cpp").read_text(encoding="utf-8")


def require(fragment: str) -> None:
    if fragment not in SOURCE:
        raise SystemExit(f"missing multiplayer item contract fragment: {fragment}")


for fragment in (
    "ITEM_STATE_AUTO_P1 = 3",
    "ITEM_STATE_AUTO_P3 = 5",
    "FixedItemState(targetPlayer->initParam)",
    "GetClosestActivePlayer(item ? &item->currentPosition : nullptr)",
    "candidates[itemIndex % count]",
    "targetPlayer->CalcItemBoxCollision",
    "ItemPlayerPower(targetPlayer)",
    "SetItemPlayerPower(targetPlayer",
    "ItemPlayerBombs(targetPlayer)",
    "SetItemPlayerBombs(targetPlayer",
    "ItemPlayerLives(targetPlayer)",
    "SetItemPlayerLives(targetPlayer",
    "targetPlayer->bombInfo.isInUse",
):
    require(fragment)

print("TH06 multiplayer item target contract: PASS")

# Match TH07MP's enemy-only LIFE/BOMB resource duplication.  The wrapper must
# not leak into deliberate player-to-player transfer SpawnItem callers.
for fragment in (
    "void ItemManager::SpawnEnemyDrop",
    "itemType == ITEM_LIFE || itemType == ITEM_BOMB",
    "MULTIPLAYER_RESOURCE_DROP_OFFSET = 16.0f",
    "GetSeparatedResourceDropPosition",
    "GetActivePlayerCount()",
    "IsPlayerActive(playerId)",
):
    require(fragment)
if "SpawnEnemyDrop" not in HEADER:
    raise SystemExit("SpawnEnemyDrop is not declared")
if "SpawnEnemyDrop" not in ENEMY:
    raise SystemExit("enemy drops do not use SpawnEnemyDrop")
if "ECL_OPCODE_DROPITEMID" not in ECL or "SpawnEnemyDrop" not in ECL:
    raise SystemExit("ECL direct drops do not use SpawnEnemyDrop")
life_transfer = PLAYER.split("void UpdateLifeTransfer", 1)[1].split("void DrawPowerTransferPrompt", 1)[0]
if "SpawnItem" not in life_transfer or "ITEM_LIFE" not in life_transfer:
    raise SystemExit("player life transfer must remain a direct SpawnItem")

print("TH06 multiplayer enemy resource duplication contract: PASS")
