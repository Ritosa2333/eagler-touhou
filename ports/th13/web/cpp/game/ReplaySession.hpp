// 回放会话恢复（R3 模块；0x447900 case1 + 0x449070 的 port 侧等价物）。
// 证据链：
//   artifacts/decomp/jp/functions/00449070.c:33-49 —— mgr+0x10==1 分支：
//     DAT_004dc324/DAT_004dc31c ← *(u16*)(stage头+2)；_DAT_004dc328=0；
//     以 4 字节为步长把 stage 头 +0x14 起 22 个 dword 连续复制到 0x4BE7B8
//     （stage_offset = 0x14 + (addr − 0x4BE7B8)，0x58 字节）。
//   artifacts/decomp/jp/functions/00447900.c:197-208 —— case1 装备序：先按
//     顶层头 +0x5C/+0x60/+0x64 写 0x4BE7B8/BC/C4（角色/机体/难度），随后被
//     同一 22-dword 复制覆盖——最终值以 stage 头为准。
//   artifacts/verification/m1-01-replay-resource-fields-20261025/evidence.md
//     （修正版字段表；不是 p60 旧表）—— 22 dword 的语义归属与未定名字段。
//   oracle/battle_oracle.py:544 附近 —— 原版恢复后 0x4DC324&0xFFFF == stage
//     头 +0x02 stamp（逐位断言，头为唯一真值源）。
// RNG 对象归属（EXE 反汇编逐调用点核验，见 HANDOFF）：
//   0x4dc324 = game RNG（ECL gvars RAND 族 0x420380/0x420d00、敌机指令
//     0x41ce10/0x417b10、弹幕出生 0x40e840/etEx 0x40f030、自机弹出膛
//     0x445c40）；0x4dc31c = ANM 指令/畸变侧第二对象（0x401000 族、
//     0x45f7c0、0x461f/0x462/0x463 族）。恢复时两者种子同置 stamp，仅
//     0x4dc324 的调用计数清零。
#pragma once

#include "ReplayFile.hpp"
#include "Rng.hpp"
#include "SessionState.hpp"

#include <cstdio>

namespace th13 {
class StageBattle;

// 恢复目标（Application 持有态；无推断字段——22 dword 中 port 无归属的
// 保持未映射并原样记录在日志里，不伪造语义）。
struct ReplayRestoreTarget {
    SessionState* progress = nullptr;
    HudPlayerState* player = nullptr;
    HudScoreState* score = nullptr;
    GameRandom* game_random = nullptr;    // 0x4dc324 等（ECL/自机 game RNG）
    GameRandom* visual_random = nullptr;  // 0x4dc31c 等（ANM 指令/畸变）
};

// 恢复日志（逐位证据；fprintf 由调用方输出）。
struct ReplayRestoreLog {
    u16 seed = 0;            // stage 头 +0x02（= 恢复后两 RNG 种子）
    u32 stage_number = 0;    // stage 头 +0x00
    i32 block[22] = {};      // 头 +0x14 起 22 dword 原样值（头内偏移 +0x14+4k）
    // 22 dword 中 port 无归属字段的头内偏移（+0x14+4k）：
    //   +0x18 机体/子角色（BC；th13 副机体不换 SHT，port 无独立字段）
    //   +0x28 未定名（CC，参与坐标/旋转运算，m1-01 表）
    //   +0x34 未定名计数（D8）
    //   +0x3C 最大得点基准（E0；port 只保留 DC/E4 两端）
    //   +0x68 灵界 UI 辅助（10C，语义未定名）
    static constexpr u32 kUnmappedOffsets[5] = {0x18, 0x28, 0x34, 0x3C, 0x68};
};

class ReplaySession {
public:
    static bool restore_player_start(const ReplayStage&,StageBattle&,std::string& error);
    static i32 stage_index(const ReplayFile& file, i32 stage_number);
    // 0x447900 case1 + 0x449070 的恢复序：顶层头 +0x5C/60/64 先写、stage 头
    // 22 dword 覆盖、种子恢复。成功返回 true；缺头/缺目标返回 false（error
    // 语义由调用方 fail 承接，这里只报布尔）。
    static bool restore_stage(const ReplayFile& file, u32 stage_index,
                              const ReplayRestoreTarget& target, ReplayRestoreLog& log,
                              std::string& error);
    // 日志行（native/web 共用 stderr 口径；oracle 侧对照值 = stamp）。
    static void emit_log(const ReplayRestoreLog& log) {
        std::fprintf(stderr,
                     "R3-RESTORE stage=%u seed=%u counter=0 block=", log.stage_number,
                     unsigned(log.seed));
        for (u32 k = 0; k < 22; ++k)
            std::fprintf(stderr, "%s%d", k ? "," : "", log.block[k]);
        std::fprintf(stderr, "\n");
    }
};

}  // namespace th13
