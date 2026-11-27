// th16 关卡表——原版 DAT_004a22d0 起 8 条 × 0xd4 字节（0x43ce10 以 DAT_004a5794 选条目写 DAT_004a6f18）。
//
//   +0x00 i32 index  +0x04 std 文件名  +0x08 ecl 文件名  +0x0c / +0x10 BGM 名（thbgm.fmt 曲目；0x42cb60 以
//   0x43c370(0, bgm0) / (1, bgm1) 装载）  +0x14..+0x20 msg 文件名 ×4（按机体）  +0x24 关卡 logo ANM
//   +0x28 i32 BGM 曲目号（0x43c3f0(0, 曲目) 播放；0x42dee0 以此解锁音乐室）  +0x2c i32 Boss 曲目号
//   +0x30 i32  +0x34 / +0x5c / +0x84 三个 Boss ANM 块（各 0x28）：{背景槽, 背景脚本, 背景不遮关卡位, 切入槽, 切入脚本,
//   对话名牌槽, 名牌脚本, 对话立绘槽, 立绘脚本, 序号}——槽号为 EnemyManager+0x164 表下标（0 bullet / 1 effect /
//   i ≥ 2 → ECL ANIM i-2）；0x417f00 以 spell_id < 0x2b 且块 1 背景槽 != -1 选块 1（道中 Boss），否则块 0；
//   MSG op 2 / 0x14 以参数选块，op 0x15 固定用块 1 立绘；0x42c480 读块 0 / 块 1 的序号。
// 条目 0（st00.ecl + st01.std）为内部测试关；正常游戏从条目 1 起。
#pragma once
#include "Types.hpp"

namespace th16 {

struct StageBossAnm {
    i32 background_slot, background_script, background_keeps_stage;  // +0x00 / +0x04 / +0x08
    i32 cutin_slot, cutin_script;                                     // +0x0c / +0x10
    i32 name_slot, name_script;                                       // +0x14 / +0x18
    i32 portrait_slot, portrait_script;                               // +0x1c / +0x20
    i32 index;                                                        // +0x24
};

struct StageEntry {
    i32 index;
    const char* std_name;
    const char* ecl_name;
    const char* bgm_name;
    const char* bgm_boss_name;
    const char* msg_names[4];
    const char* logo_anm;
    i32 bgm_track;
    i32 bgm_boss_track;
    i32 reserved_30;
    StageBossAnm boss_anm[3];  // +0x34 / +0x5c / +0x84
};

inline constexpr StageEntry kStageTable[8] = {
    {0, "st01.std", "st00.ecl", "th16_02", "th16_03", {"st00a.msg", "st01b.msg", "st01c.msg", "st01d.msg"}, "st01logo.anm", 1, 2, 0, {{3, 9, 0, 3, 17, 3, 14, 3, 8, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}},
    {1, "st01.std", "st01.ecl", "th16_02", "th16_03", {"st01a.msg", "st01b.msg", "st01c.msg", "st01d.msg"}, "st01logo.anm", 1, 2, 0, {{3, 9, 0, 3, 17, 3, 14, 3, 8, 0}, {3, 9, 0, 3, 17, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}},
    {2, "st02.std", "st02.ecl", "th16_04", "th16_05", {"st02a.msg", "st02b.msg", "st02c.msg", "st02d.msg"}, "st02logo.anm", 3, 4, 0, {{3, 11, 0, 3, 19, 3, 16, 3, 10, 1}, {-1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}},
    {3, "st03.std", "st03.ecl", "th16_06", "th16_07", {"st03a.msg", "st03b.msg", "st03c.msg", "st03d.msg"}, "st03logo.anm", 5, 6, 0, {{3, 11, 0, 3, 20, 3, 17, 3, 10, 3}, {3, 11, 0, -1, -1, -1, -1, -1, -1, 2}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}},
    {4, "st04.std", "st04.ecl", "th16_08", "th16_09", {"st04a.msg", "st04b.msg", "st04c.msg", "st04d.msg"}, "st04logo.anm", 7, 8, 0, {{3, 12, 0, 3, 19, 3, 16, 3, 11, 4}, {4, 12, 0, 4, 19, 4, 16, 4, 11, -1}, {4, 12, 0, 4, 19, 4, 16, 4, 11, -1}}},
    {5, "st05.std", "st05.ecl", "th16_10", "th16_11", {"st05a.msg", "st05b.msg", "st05c.msg", "st05d.msg"}, "st05logo.anm", 9, 10, 0, {{3, 12, 0, 3, 19, 3, 16, 3, 11, 5}, {-1, -1, 0, 3, 20, -1, -1, 4, 8, -1}, {-1, -1, 0, 3, 21, -1, -1, 4, 8, -1}}},
    {6, "st06.std", "st06.ecl", "th16_13", "th16_12", {"st06a.msg", "st06b.msg", "st06c.msg", "st06d.msg"}, "st06logo.anm", 11, 12, 0, {{3, 10, 0, 3, 17, 3, 14, 3, 9, 6}, {-1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}},
    {7, "st07.std", "st07.ecl", "th16_16", "th16_17", {"st07a.msg", "st07b.msg", "st07c.msg", "st07d.msg"}, "st07logo.anm", 13, 14, 0, {{3, 13, 0, 3, 20, 3, 17, 3, 12, 6}, {4, 9, 1, 4, 13, -1, -1, 4, 8, 5}, {-1, -1, 0, -1, -1, -1, -1, 5, 8, -1}}},
};

// DAT_004a6f18 = &DAT_004a22d0 + DAT_004a5794 * 0xd4
inline const StageEntry& stage_entry(i32 stage) {
    return kStageTable[(stage < 0 || stage > 7) ? 0 : stage];
}

}  // namespace th16
