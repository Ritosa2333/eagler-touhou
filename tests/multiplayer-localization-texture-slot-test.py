from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]
LOCALIZATION = (ROOT / "src/Localization.cpp").read_text(encoding="utf-8")
ANM = (ROOT / "src/AnmIdx.hpp").read_text(encoding="utf-8")
MANAGER = (ROOT / "src/AnmManager.cpp").read_text(encoding="utf-8")


def define(source: str, name: str) -> int:
    match = re.search(
        rf"^#define\s+{re.escape(name)}\s+(0x[0-9a-fA-F]+|\d+)",
        source,
        flags=re.MULTILINE,
    )
    if not match:
        raise AssertionError(f"missing define {name}")
    return int(match.group(1), 0)


def constexpr(name: str) -> int:
    match = re.search(rf"constexpr i32\s+{name}\s*=\s*(\d+);", LOCALIZATION)
    if not match:
        raise AssertionError(f"missing constexpr {name}")
    return int(match.group(1))


localized = {
    constexpr("TEXTURE_SLOT_LOCALIZED_STAGE"),
    constexpr("TEXTURE_SLOT_LOCALIZED_MUSIC"),
    constexpr("TEXTURE_SLOT_LOCALIZED_BOSS_TITLE"),
    constexpr("TEXTURE_SLOT_LOCALIZED_BOSS_NAME"),
}
multiplayer = {
    define(ANM, "ANM_FILE_PLAYER2"),
    define(ANM, "ANM_FILE_PLAYER3"),
    define(ANM, "ANM_FILE_FACE2_CHARA_A"),
    define(ANM, "ANM_FILE_FACE2_CHARA_B"),
    define(ANM, "ANM_FILE_FACE2_CHARA_C"),
    define(ANM, "ANM_FILE_FACE3_CHARA_A"),
    define(ANM, "ANM_FILE_FACE3_CHARA_B"),
    define(ANM, "ANM_FILE_FACE3_CHARA_C"),
}

assert localized.isdisjoint(multiplayer)
assert len(localized) == 4
assert localized == set(range(252, 256))
assert multiplayer == set(range(256, 264))
for slot in localized:
    assert f"ApplyTextImage(vm, 0x7fe, {slot}" not in LOCALIZATION
assert "ApplyTextImage(vm, 0x7fe, TEXTURE_SLOT_LOCALIZED_STAGE" in LOCALIZATION
assert "ApplyTextImage(vm, 0x7ff, TEXTURE_SLOT_LOCALIZED_MUSIC" in LOCALIZATION
assert "textureIdx < ARRAY_SIZE_SIGNED(this->anmFiles)" in MANAGER
assert "textureIdx >= ARRAY_SIZE_SIGNED(this->textures)" in MANAGER

print("TH06 multiplayer/localization texture slots: PASS")
