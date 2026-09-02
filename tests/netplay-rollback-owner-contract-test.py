from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

rollback = (ROOT / "src/netplay/Th06RollbackState.cpp").read_text(encoding="utf-8")
header = (ROOT / "src/netplay/Th06RollbackState.hpp").read_text(encoding="utf-8")
bullet = (ROOT / "src/BulletManager.cpp").read_text(encoding="utf-8")
enemy = (ROOT / "src/EnemyManager.cpp").read_text(encoding="utf-8")
item = (ROOT / "src/ItemManager.cpp").read_text(encoding="utf-8")
effect = (ROOT / "src/EffectManager.cpp").read_text(encoding="utf-8")
player_h = (ROOT / "src/Player.hpp").read_text(encoding="utf-8")
bullet_h = (ROOT / "src/BulletManager.hpp").read_text(encoding="utf-8")
enemy_h = (ROOT / "src/EnemyManager.hpp").read_text(encoding="utf-8")
item_h = (ROOT / "src/ItemManager.hpp").read_text(encoding="utf-8")
effect_h = (ROOT / "src/EffectManager.hpp").read_text(encoding="utf-8")
sound = (ROOT / "src/SoundPlayer.cpp").read_text(encoding="utf-8")
supervisor = (ROOT / "src/Supervisor.cpp").read_text(encoding="utf-8")
replay = (ROOT / "src/ReplayManager.cpp").read_text(encoding="utf-8")
enemy_ecl = (ROOT / "src/EnemyEclInstr.cpp").read_text(encoding="utf-8")
enemy_ecl_h = (ROOT / "src/EnemyEclInstr.hpp").read_text(encoding="utf-8")


def require(ok: bool, label: str) -> None:
    if not ok:
        raise AssertionError(label)


# Concrete TH06 storage, not copied TH07 capacities.
require("Enemy enemies[257]" in enemy_h, "TH06 enemy capacity")
require("Bullet bullets[640]" in bullet_h, "TH06 bullet capacity")
require("Laser lasers[64]" in bullet_h, "TH06 laser capacity")
require("Item items[513]" in item_h, "TH06 item capacity")
require("Effect effects[513]" in effect_h, "TH06 effect capacity")
require("PlayerBullet bullets[80]" in player_h, "TH06 player bullet capacity")

require("CHECKPOINT_LOGICAL_FRAMES = 2" in rollback, "two-frame first-write checkpoints")
require("CaptureStageMutableState" in rollback, "TH06 stage mutable owner")
require("g_Stage.quadVms" in rollback, "stage ANM VM rollback")
require("TouchObject(&object->flags)" in rollback, "stage object execution flag rollback")
require("struct RollbackState" in enemy_ecl_h, "cross-frame EnemyEclInstr rollback owner")
require("static RollbackState g_RollbackState{};" in enemy_ecl,
        "star-pattern state consolidated behind rollback owner")
require("TouchObject(EnemyEclInstr::GetRollbackState())" in rollback,
        "EnemyEclInstr star-pattern sidecar rollback")
require("ScreenEffect::ShakeScreen" in rollback and "RestoreScreenEffects" in rollback,
        "dynamic ScreenEffect chain rollback")
require("maxScreenEffectsPerFrame" in header, "bounded ScreenEffect snapshot")

require("Th06Rollback::TouchBullet(bullet)" in bullet, "bullet reusable-slot hook")
require("Th06Rollback::TouchLaser(laser)" in bullet, "laser reusable-slot hook")
require("Th06Rollback::TouchEnemy(newEnemy)" in enemy, "enemy reusable-slot hook")
require("Th06Rollback::TouchItem(item)" in item, "item reusable-slot hook")
require("Th06Rollback::TouchEffect(effect)" in effect, "effect reusable-slot hook")

require("SideEffects::IsSpeculative()" in sound, "speculative SFX suppression")
require(supervisor.count("SideEffects::IsSpeculative()") >= 4,
        "speculative BGM/MIDI suppression")
require("SideEffects::IsSpeculative()" in replay, "speculative replay-write suppression")

require("Th07Rollback" not in rollback and "TH07_MULTI" not in rollback,
        "no TH07 owner identity leaked into TH06 rollback state")

print("TH06 rollback owner contract: PASS")
