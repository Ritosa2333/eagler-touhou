#!/usr/bin/env python3
"""Source-level guard that shared presentation optimizations stay out of gameplay."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
bullet_cpp = (ROOT / "src/BulletManager.cpp").read_text(encoding="utf-8")
bullet_hpp = (ROOT / "src/BulletManager.hpp").read_text(encoding="utf-8")
item_cpp = (ROOT / "src/ItemManager.cpp").read_text(encoding="utf-8")
item_helper = (ROOT / "src/graphics/ItemPresentation.hpp").read_text(encoding="utf-8")
anm_cpp = (ROOT / "src/AnmManager.cpp").read_text(encoding="utf-8")
anm_vm = (ROOT / "src/AnmVm.hpp").read_text(encoding="utf-8")


def require(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    print("PASS:", name)


update_live = bullet_hpp.split("void UpdateLivePrev", 1)[1].split("void UpdatePrev", 1)[0]
require("live-prev helper only delegates UpdatePrev and switches on state",
        "UpdatePrev();" in update_live and "=" not in update_live)
require("normal and despawn endpoints are always published",
        "spriteBullet.UpdatePrev();" in update_live and "spriteSpawnEffectDonut.UpdatePrev();" in update_live)
for state, vm in [
    ("BULLET_SPAWNING_FAST", "spriteSpawnEffectFast"),
    ("BULLET_SPAWNING_NORMAL", "spriteSpawnEffectNormal"),
    ("BULLET_SPAWNING_SLOW", "spriteSpawnEffectSlow"),
]:
    require(f"{state} publishes its visible spawn VM", f"case {state}: {vm}.UpdatePrev();" in update_live)

spawn_tail = bullet_cpp.split("bullet->RunCommands();", 1)[1].split("return 0;", 1)[0]
require("bullet slot spawn fully initializes every presentation endpoint",
        "bullet->sprites.UpdatePrev();" in spawn_tail)

draw = bullet_cpp.split("void Bullet::Draw()", 1)[1].split("u32 BulletManager::OnDraw", 1)[0]
require("bullet Draw selects one VM from state only",
        all(token in draw for token in ["BULLET_SPAWNING_FAST", "BULLET_SPAWNING_NORMAL",
                                        "BULLET_SPAWNING_SLOW", "BULLET_DESPAWN", "spriteBullet"]))

# prev* fields are presentation endpoints. The renderer may read them; authored
# script execution must not consume them as game state.
anm_vm_cpp = (ROOT / "src/AnmVm.cpp").read_text(encoding="utf-8")
for field in ("prevRotation", "prevScale", "prevUvScrollPos", "prevColor", "prevColor2", "prevUseColor2"):
    lines = [line.strip() for line in anm_vm_cpp.splitlines() if field in line]
    require(f"ANM script implementation only initializes {field}, never reads it",
            all("=" in line and line.find(field) < line.find("=") for line in lines))
require("ANM renderer owns previous endpoint reads", "prevScale" in anm_cpp and "prevColor" in anm_cpp)

draw_body = item_cpp.split("void ItemManager::OnDraw()", 1)[1]
for forbidden in ("isOnscreen", "SetActiveSprite", "sprite.color.color", "zWriteDisable"):
    require(f"Item Draw does not mutate persistent appearance via {forbidden}", forbidden not in draw_body)
require("Item Draw restores temporary interpolated position", "item->sprite.pos = authoredPos;" in draw_body)

update_body = item_cpp.split("void ItemManager::OnUpdate()", 1)[1].split("void ItemManager::RemoveAllItems", 1)[0]
require("item appearance update follows authored ANM ExecuteScript in fixed tick",
        update_body.index("ExecuteScript(&item->sprite)") < update_body.index("Graphics::UpdateItemPresentation"))
require("item appearance helper has no score/collision/player/RNG operations",
        not any(token in item_helper for token in ("score", "collision", "g_Player", "g_Rng", "timer")))

other_sources = "\n".join(
    path.read_text(encoding="utf-8", errors="ignore")
    for path in (ROOT / "src").glob("*.cpp") if path.name != "ItemManager.cpp"
)
require("Item isOnscreen is not read by other gameplay translation units", "isOnscreen" not in other_sources)

print("single-player presentation purity: PASS")
