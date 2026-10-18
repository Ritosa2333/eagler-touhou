from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[2]
anm = (ROOT / "src/AnmIdx.hpp").read_text(encoding="utf-8")
player_h = (ROOT / "src/Player.hpp").read_text(encoding="utf-8")
manager_h = (ROOT / "src/AnmManager.hpp").read_text(encoding="utf-8")
player = (ROOT / "src/Player.cpp").read_text(encoding="utf-8")
main_menu = (ROOT / "src/MainMenu.cpp").read_text(encoding="utf-8")
music_room = (ROOT / "src/MusicRoom.cpp").read_text(encoding="utf-8")
result = (ROOT / "src/ResultScreen.cpp").read_text(encoding="utf-8")
ending = (ROOT / "src/Ending.cpp").read_text(encoding="utf-8")


def require(ok: bool, label: str) -> None:
    if not ok:
        raise AssertionError(label)


def define(name: str) -> int:
    match = re.search(
        rf"^#define\s+{re.escape(name)}\s+(0x[0-9a-fA-F]+|\d+)",
        anm,
        flags=re.MULTILINE,
    )
    if not match:
        raise AssertionError(f"missing define {name}")
    return int(match.group(1), 0)


require("#define ANM_FILE_STAFF03 46" in anm, "original highest named ANM file slot")
require("#define ANM_FILE_PLAYER2 256" in anm and "#define ANM_FILE_PLAYER3 257" in anm,
        "guest player files use the reserved high slots")
require("#define ANM_OFFSET_PLAYER2 0x4c0" in anm, "P2 audited player bank")
require("#define ANM_OFFSET_PLAYER3 0x550" in anm, "P3 audited player bank")
require("#define ANM_SPRITE_FACE_STAGE_END 0x4ab" in anm, "original face bank end")
require("#define ANM_OFFSET_FRONT 0x600" in anm, "original front bank start")
require("#define ANM_SCRIPT_PLAYER_END 0x48c" in anm, "original audited player bank end")
require("AnmRawEntry *anmFiles[264]" in manager_h, "ANM file slot capacity")
require("const AnmRawInstr *scripts[2048]" in manager_h and "AnmLoadedSprite sprites[2048]" in manager_h,
        "script/sprite index capacity")
require("Player g_Players[TH06_MULTI_MAX_PLAYERS]" in player_h,
        "stable multiplayer Player storage")
require("u8 initParam" in player_h and "u8 unk_9e1" in player_h,
        "layout-preserving MP slot id alias")

# Raw player script span is 0x8c inclusive from base. Prove both sidecars fit
# the original unused 0x4ac..0x5ff interval and do not touch each other.
p2_start, p3_start, span, front = 0x4C0, 0x550, 0x8C, 0x600
require(p2_start + span < p3_start, "P2/P3 player banks do not overlap")
require(p3_start + span < front, "P3 player bank stays below original front bank")

# Protect the exact original TH06 menu/file layout that the old TH06MP port got
# wrong.  Title/select/replay/result/music/staff occupy every file slot 21..46;
# the new guest player files must stay strictly above that complete range.
menu_file_names = [
    "ANM_FILE_TITLE01", "ANM_FILE_TITLE01S", "ANM_FILE_TITLE04S",
    "ANM_FILE_TITLE02", "ANM_FILE_TITLE03", "ANM_FILE_TITLE04",
    "ANM_FILE_SELECT01", "ANM_FILE_SELECT02", "ANM_FILE_SELECT03",
    "ANM_FILE_SELECT04", "ANM_FILE_SELECT05",
    "ANM_FILE_SLPL00A", "ANM_FILE_SLPL00B", "ANM_FILE_SLPL01A", "ANM_FILE_SLPL01B",
    "ANM_FILE_REPLAY",
    "ANM_FILE_RESULT00", "ANM_FILE_RESULT01", "ANM_FILE_RESULT02", "ANM_FILE_RESULT03",
    "ANM_FILE_MUSIC00", "ANM_FILE_MUSIC01", "ANM_FILE_MUSIC02",
    "ANM_FILE_STAFF01", "ANM_FILE_STAFF02", "ANM_FILE_STAFF03",
]
menu_slots = sorted(define(name) for name in menu_file_names)
require(menu_slots == list(range(21, 47)), "original Title/Select/Replay/Result/Music/Staff slots remain 21..46")
require(define("ANM_FILE_PLAYER2") == 256 and define("ANM_FILE_PLAYER3") == 257,
        "guest player files begin in the reserved high block")
require(define("ANM_FILE_FACE3_CHARA_C") == 263,
        "guest player and face files occupy the top eight slots")

# The source must use the audited per-slot files/offsets and release the same
# file on destruction.  This is important because a stale guest binding could
# survive gameplay and collide with a later menu transition even with unique
# numeric file slots.
require("case 1: return ANM_FILE_PLAYER2;" in player and "case 2: return ANM_FILE_PLAYER3;" in player,
        "Player maps guests to independent ANM file slots")
require("case 1: return ANM_OFFSET_PLAYER2;" in player and "case 2: return ANM_OFFSET_PLAYER3;" in player,
        "Player maps guests to independent ANM script banks")
require("g_AnmManager->ReleaseAnm(PlayerAnmFile(p));" in player,
        "Player releases the same slot it loaded")

# Protect the real post-game/menu users that historically exposed slot
# collisions.  Web title prewarming deliberately touches Replay, Result, and
# Music assets, while Ending owns the Staff range.  None of these may be moved
# into the guest slots.
for file_name in ("ANM_FILE_RESULT00", "ANM_FILE_RESULT01", "ANM_FILE_RESULT02", "ANM_FILE_RESULT03",
                  "ANM_FILE_MUSIC00", "ANM_FILE_MUSIC01", "ANM_FILE_MUSIC02", "ANM_FILE_REPLAY"):
    require(file_name in main_menu, f"MainMenu transition/prewarm keeps {file_name}")
for file_name in ("ANM_FILE_MUSIC00", "ANM_FILE_MUSIC01", "ANM_FILE_MUSIC02"):
    require(f"LoadAnm({file_name}" in music_room, f"Music Room loads {file_name}")
for file_name in ("ANM_FILE_RESULT00", "ANM_FILE_RESULT01", "ANM_FILE_RESULT02", "ANM_FILE_RESULT03"):
    require(f"LoadAnm({file_name}" in result, f"ResultScreen loads {file_name}")
for file_name in ("ANM_FILE_STAFF01", "ANM_FILE_STAFF02", "ANM_FILE_STAFF03"):
    require(f"LoadAnm({file_name}" in ending, f"Ending loads {file_name}")

require("for (i = ANM_FILE_SELECT01; i <= ANM_FILE_REPLAY; i++)" in main_menu,
        "Title transition release range stops at Replay and cannot release guest slots")

print("TH06 player resource bank contract: PASS")
