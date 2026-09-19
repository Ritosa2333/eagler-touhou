from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[2]
LOCALIZATION = (ROOT / "src/Localization.cpp").read_text(encoding="utf-8")
ANM = (ROOT / "src/AnmIdx.hpp").read_text(encoding="utf-8")
TEXT = (ROOT / "src/TextHelper.cpp").read_text(encoding="utf-8")
PLAYER = (ROOT / "src/Player.cpp").read_text(encoding="utf-8")
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
    constexpr("TEXTURE_SLOT_LOCALIZED_BOSS_TITLE"),
    constexpr("TEXTURE_SLOT_LOCALIZED_BOSS_NAME"),
}
multiplayer = {
    define(ANM, "ANM_FILE_PLAYER2"),
    define(ANM, "ANM_FILE_PLAYER3"),
    define(ANM, "ANM_FILE_FACE2"),
    define(ANM, "ANM_FILE_FACE2") + 1,
    define(ANM, "ANM_FILE_FACE3"),
    define(ANM, "ANM_FILE_FACE3") + 1,
}

assert localized.isdisjoint(multiplayer)
assert len(localized) == 2
assert localized == {248, 249}
assert multiplayer == set(range(250, 256))
assert "SetCurrentTexture(outTexture);" in TEXT
assert "missileAnmIdx -= GetPlayerAnmScript(player, ANM_OFFSET_PLAYER)" in PLAYER
assert MANAGER.count("textureIdx >= static_cast<i32>(ARRAY_SIZE(this->imageDataArray))") >= 4
assert "textureIdx < 0 || textureIdx >= ANM_FILE_SLOT_COUNT" in MANAGER

print("TH07 multiplayer/localization texture slots: PASS")
