#pragma once
#include "PortTitle.hpp"
#include "ShotFiles.hpp"
#include "Types.hpp"
#include <cstdio>

namespace th11 {

// Which files one stage loads. Every name here is read from the title's own archive, not inferred
// from TH12's, and the *roles* are read from what references what:
//
//   node tools/th-archive-names.mjs 12 12 .std    -> stage01..07.std
//   node tools/th-archive-names.mjs 13 13 .std    -> st01..07.std
//   node tools/th-archive-names.mjs 12 12 .ecl    -> stageNN.ecl, stage07boss.ecl, default.ecl
//   node tools/th-archive-names.mjs 13 13 .ecl    -> stNN.ecl, stNNbs.ecl, stNNmbs.ecl, default.ecl
//   node tools/th-archive-names.mjs 12 12 .msg    -> stNN_MMa.msg / stNN_MMb.msg, three pairs
//   node tools/th-archive-names.mjs 13 13 .msg    -> stNNa.msg .. stNNd.msg, four
//
// The background and enemy ANMs are not named the same way, and the naming does not say which is
// which. The scene scripts do: TH12's `stage01.std` holds the string `stage01.anm` and TH13's
// `st01.std` holds `st01wl.anm`, so the wall ANM is the background in TH13 exactly as
// `stageNN.anm` is in TH12; and TH12's ECL refers to `stgenm01.anm` (the port loads it as the
// enemy ANM) while TH13's `st01.ecl` refers to `st01enm.anm`, so that is the enemy ANM for
// TH13. The `logo` name is the one both titles share.
//
// Dialogues differ in shape, not only in name: TH12 pairs them (`_00a`/`_00b`, three pairs per
// stage) and TH13 letters them (`a` to `d`). A title whose set differs is a row here.
struct StageFiles {
    const char* scene;      // the STD background script
    const char* background; // the background ANM the scene script references
    const char* logo;       // the stage logo ANM
    const char* enemies;    // the general-enemy ANM the stage ECL references
    const char* timeline;   // the stage ECL
    const char* boss;       // the boss ECL (absent for most stages; the loader tolerates that)
    const char* midboss;    // the midboss ECL
    const char* dialogue;   // one message per index
    u32 dialogue_count;     // how many a stage ships
    bool dialogue_pairs;     // TH12 numbers them stNN_MMa; TH13 letters them stNNa
};

inline constexpr StageFiles stage_files_th12={"stage%02u.std","stage%02u.anm","st%02ulogo.anm","stgenm%02u.anm",
                                           "stage%02u.ecl","stage%02uboss.ecl","stage%02umboss.ecl",
                                           "st%02u_%02u%c.msg",6,true};
inline constexpr StageFiles stage_files_th13={"st%02u.std","st%02uwl.anm","st%02ulogo.anm","st%02uenm.anm",
                                           "st%02u.ecl","st%02ubs.ecl","st%02umbs.ecl",
                                           "st%02u%c.msg",4,false};
// TH14 STD headers name stNNwl.anm; stNN.ecl references stNNenm.anm.
// Its six lettered messages cover character * 2 + equipment.
inline constexpr StageFiles stage_files_th14={"st%02u.std","st%02uwl.anm","st%02ulogo.anm","st%02uenm.anm",
                                           "st%02u.ecl","st%02ubs.ecl","st%02umbs.ecl",
                                           "st%02u%c.msg",6,false};

// TH16, TH17 and TH18 all keep TH13's naming, and each one's own archive and own scripts say so:
//   node tools/th-archive-names.mjs 16 14 .std   -> st01..07.std
//   the dumped st01.std of each title holds `st01wl.anm`, and its dumped st01.ecl holds
//   `st01enm.anm`, which is the background/enemy pair TH13 and TH14 name too.
// The message set is what separates them, and each is one lettered file per index:
//   th16 st01a..st01d (4), th17 st01a..st01i (9), th18 st01a..st01d (4),
// against 4 for TH13 and 6 (character*2+equipment) for TH14.
inline constexpr StageFiles stage_files_th16={"st%02u.std","st%02uwl.anm","st%02ulogo.anm","st%02uenm.anm",
                                           "st%02u.ecl","st%02ubs.ecl","st%02umbs.ecl",
                                           "st%02u%c.msg",4,false};
inline constexpr StageFiles stage_files_th17={"st%02u.std","st%02uwl.anm","st%02ulogo.anm","st%02uenm.anm",
                                           "st%02u.ecl","st%02ubs.ecl","st%02umbs.ecl",
                                           "st%02u%c.msg",9,false};
inline constexpr StageFiles stage_files_th18={"st%02u.std","st%02uwl.anm","st%02ulogo.anm","st%02uenm.anm",
                                           "st%02u.ecl","st%02ubs.ecl","st%02umbs.ecl",
                                           "st%02u%c.msg",4,false};
inline constexpr const StageFiles& stage_files = kPortTitle==18?stage_files_th18:kPortTitle==17?stage_files_th17
    :kPortTitle==16?stage_files_th16:kPortTitle==14?stage_files_th14:kPortTitle==13?stage_files_th13
    :stage_files_th12;

// The name of one dialogue file, since the two shapes take different arguments. The original reads
// them together - one message file per speaker turn - so an index is all a caller has.
// Which of a stage's message files belongs to a selection. TH12 ships two per character (the A/B
// shot types, six files, named stNN_MMa/stNN_MMb) and numbers the selection `character*2+subtype`;
// TH13 ships one per character (four files, stNNa..stNNd) and the measurement of its player
// rows is `player_subtype_max = 0` (ShotFiles.hpp), so the selection index is the character alone.
// Passing TH12's expression to TH13 reaches past its four files, which is what the boot measured:
// `[diag] character=0 subtype=0 index=0 files=6`.
// The count of a title's message set is the same product its shot table uses, so the index is
// that product and not a rule per title: characters x loadouts is 3x2=6, 4x1=4, 3x2=6, 4x1=4,
// 3x3=9 and 4x1=4 for 12/13/14/16/17/18, and `shot_loadout_count` (ShotFiles.hpp) is the
// measured second factor of every one of them.
inline u32 stage_dialogue_index(u32 character, u32 subtype) {
    return character * shot_loadout_count + subtype;
}

template <typename Buffer>
inline void stage_dialogue_name(Buffer& out, u32 capacity, u32 stage, u32 index) {
    if (stage_files.dialogue_pairs)
        std::snprintf(out, capacity, stage_files.dialogue, stage, index / 2, char('a' + index % 2));
    else
        std::snprintf(out, capacity, stage_files.dialogue, stage, char('a' + index));
}

}
