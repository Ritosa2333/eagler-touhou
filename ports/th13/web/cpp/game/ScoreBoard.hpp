// th13 计分公式族（S3 第一阶段；docs/plans/results-overlay-anatomy.md §3 的
// 字节级直抄，锚点 0x42abf0 / 0x42cd40 / 0x42ce96-0x42ced0 / 0x42cfb3）。
//
// 点值制：内部分 DAT_004be7c0 = 显示分/10（尾位显示另补），所有 bonus 以
// bonus/10 记入，封顶 999,999,999（0x3b9ac9ff，比较 0x3b9aca00 @0x42cef3/
// 0x42cfe7）。移植侧 HudScoreState.score 即该点值（StageBattle kKillScore
// 同约定）。
#pragma once

#include "Types.hpp"

namespace th13 {

// 每关通关奖（0x42abf0:29-37）：bonus = 关号 × 1,000,000（关号取递增前的
// 当前关 DAT_004be81c；stage ctx+0x2ac 记 bonus 原值供 HUD 通关演出）。
constexpr i32 stage_clear_bonus(i32 stage) { return stage * 1000000; }

// 六面全通奖四难度跳表（0x42cd40 switch DAT_004be7c4，0x42ce96-0x42ced0）：
// 残 = DAT_004be7f4、灵 = DAT_004be800。
constexpr i32 all_clear_bonus(i32 difficulty, i32 lives, i32 bombs) {
    switch (difficulty) {
        case 0:
            return (bombs + lives * 4) * 250000;
        case 1:
            return (bombs + lives * 3) * 1000000;
        case 2:
            return lives * 5000000 + bombs * 1500000;
        case 3:
            return (bombs + lives * 5) * 2000000;
        default:
            return 0;
    }
}

// Extra（st7）通关奖（0x42cfb3-c4）：(灵 + 残×10) × 4,000,000。
constexpr i32 extra_clear_bonus(i32 lives, i32 bombs) {
    return (bombs + lives * 10) * 4000000;
}

// 入账：score（点值）+= bonus/10，封顶 999,999,999（64 位中间量防环绕）。
inline u32 add_bonus_capped(u32 score, i32 bonus) {
    const i64 next = i64(score) + bonus / 10;
    if (next < 0) return 0;
    if (next > 999999999) return 999999999;
    return u32(next);
}

}  // namespace th13
