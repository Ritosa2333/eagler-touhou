// 对局进度归属（最小集，docs/plans/ui-base.md §三）：
// 进度属于 run，穿越场景工厂；区别于逐帧输入、显示状态与存档记录。
#pragma once

#include "Types.hpp"

namespace th13 {

struct SessionState {
    i32 stage = 1, stage_frame = 0, character = 0;
    i32 difficulty = 0, continues = 0;
    // DAT_004be844: per-difficulty remaining continues, initialized from
    // 0x4c0ae0 at 42b990 and decremented by the continue decision.
    i32 continue_stock = 3;
    // hi-score（0x4be810，点值制）：新档默认 = score file 行 hi 100000
    //（0x449760 默认表首行 100000；0x403fd0 显示 ×10 ⇒ 1,000,000）。
    // Application 按存档状态预置；对局中只增（0x42a370/0x43d650）。
    i32 high_score = 100000, high_score_extra = 0;
    i32 configured_lives = 3;
    i32 starting_stage = 1;
    i32 chapter = 0, chapter_frame = 0; // be824/be82c, 524 ->424310.
    float rate = 1;
    u32 scene_flags = 0;
    bool new_run = true;
    // ---- S3 对局终了链路（results-overlay-anatomy.md / results-flow.md）----
    // DAT_004be830：bit4-5 = mode（0=故事/Extra 1=关卡练习 2=符卡练习）、
    // bit6 = 演示回放；0x42cd40 以 (flags>>4)&3 取 mode、&0x30==0x20 门符卡练习。
    u32 run_mode_flags = 0;
    // GameInf+0x74 场景参数（0=普通开局，1=Replay/练习入口）：
    // 43c1c0 case0xd →42c490(1)；非0时终局改走结算A（43e5a0）。
    i32 practice_mode = 0;
    // DAT_004be7d4: -1 for ordinary/Stage Practice, selected card for Spell Practice.
    // Both ECL getters (00420380/00420d00) read this same session value.
    i32 selected_spell_id = -1;
    // DAT_004be808：灵界（trance）计，0..600（每 200 一魂；0x42b030 段划分，
    // 0x4aeca8=3.0 / 0x4aece8=200.0）。asm 0x43fa90:546 与 0x4406b6 是同一
    // 条续关决策写（results-flow.md §3：光标0 续关 → be7c8++/be844--，同时
    // be808=200），不是新局；新局写点未找到，保留 .data 原初值 0。
    // 收集小灵 0x438730 公共 +2（0x438b3d lea -0x3d(edi)=2），类型 1 条件 +1、
    // 类型 3 +1；0x43d530 累加封顶 600；0x405b80:78 正常结束 -200；
    // 0x42b990:121 仅练习模式清零。
    // 同时是终局门（0x42cd40:34：>599 → 好结局资格）。字段名沿用旧登记
    // 「白点/ending_points」，跨 owner 重命名 pending，语义以本注释为准。
    i32 ending_points = 0;
    // DAT_004be83c（好结局资格计数；灵击≥3 时置 3，结局选择门见 results-flow.md §4.2）。
    i32 good_endings = 0;
    // GameInf+0x2ac：本次关终 clear bonus 累计（0x42abf0 覆盖写、六面/Extra
    // 再 += 同式原值；HUD 通关演出消费——显示 TODO）。
    i32 stage_clear_bonus = 0;
    // score 数据块 +0x7fc 起的关卡通过位（内存态；ScoreFile 落档属后期阶段）。
    // 位序 = stage + difficulty*8（0x42cd40:100 的字节写位同式）。
    u32 stage_cleared_bits = 0;
    // ---- th13 对局 HUD 数值通道（原版 DAT_004beXXX 家族的会话态）----
    // 最大得点/点值（DAT_004be7dc）：初值 = 0x42b990:107-110 两条表写中的
    // 0x4c0ab0[diff]×100（EXE .data 实读 5000/10000/15000/20000 → be7e0；
    // 0x42bb8c 起字节 db 05 fimul [be7e0] + fld 100.0 + fdiv + ×100 同值往返
    // + call 0x4971f0 ftol → be7dc，即 Ghidra「float 参数」误读源）。
    // 封顶 DAT_004be7e4 = 0x4c0ac8[diff]×100（100000/250000/500000/500000 →
    // 10000000/25000000/50000000/50000000，Extra 行 4）。
    // 增量 = 擦弹计数路径 +100/次（0x445900:14；调用方 0x4315d0:172 按 %3
    // 门控、0x40db10:228/0x419a90:299 弹幕碰撞类），非敌击破。
    // HUD 显示值 = 当前/100 再抹个位（0x427910:236），Normal 开局屏显 10000
    //（demo-original-live/stage1-check2.png 实证），封顶显示 250000。
    i32 max_point = 0, max_point_cap = 0;
    // Graze（DAT_004be7d0）：每弹幕擦弹 +1（0x445900:11-13，调用方为弹幕
    // 碰撞类，非敌击破计数），封顶 99,999,999。0x42b990:103 关首清零。
    i32 graze = 0;
    // ---- 残机/灵击碎片分母（DAT_004be7fc = 0x4be7b8+0x44）----
    // 阈值下标，0x43d330 每获得一残机 +1（结构体 +0x44，:56 无条件递增）。
    // 表 0x4bb994（EXE .data 实读 {8,10,12,15,18,20,25,9999999}）：
    // 第一命 8 碎片，此后逐命递增；末项 9999999 是「无碎片」哨兵，
    // 0x427910:170/187 据此显示 ---/---。Normal 开局下标 0 → HUD 0 / 8
    // （demo-original-live/passive-audio-obs/stage1-check2.png 实证）。
    i32 life_piece_threshold_index = 0;
};

// E/N/H/L/Extra/Overdrive occupy six distinct ECL rank bits.
inline u8 ecl_difficulty_mask(i32 difficulty) noexcept {
    return difficulty >= 0 && difficulty < 6 ? u8(1u << difficulty) : 0;
}

// Original 0x42b990:70-72 records stage-played only for an ordinary run.
// Replay/Practice/Spell/Extra must not manufacture Practice unlocks.
inline bool records_practice_stage(const SessionState& state, bool replay) noexcept {
    return !replay && state.practice_mode == 0 && (state.run_mode_flags & 0x70u) == 0 &&
        state.character >= 0 && state.character < 4 &&
        state.difficulty >= 0 && state.difficulty < 4 &&
        state.stage >= 1 && state.stage <= 6;
}

// Returns-to-title from a real Practice run re-enters Stage Select (original
// 0x414fc0(4) keeps DAT_004dc670 at its pre-title value: 0x450fb0 case2→0x451450
// case2 only when dc670==4, written by 0x451cb0:159 on the way in). Covers both
// pause-menu Exit (0x43e780:209) and GameOver Continue-menu exit (0x43fa90:
// 549-552 / 0x440960 exit-to-title) — all issue the same scene request 4.
// Ordinary-run, replay/demo and cold-boot returns keep their existing paths:
// dc670 is 1/2/5/0 there, never 4.
inline bool returns_to_practice_selection(const SessionState& state, i32 reason,
                                          bool replay) noexcept {
    return (reason == 1 || reason == 3) && !replay && state.practice_mode != 0 &&
        (state.run_mode_flags & 0x70u) == 0x10u &&
        state.character >= 0 && state.character < 4 &&
        state.difficulty >= 0 && state.difficulty < 4 &&
        state.starting_stage >= 1 && state.starting_stage <= 6;
}

// 0x4bb994 阈值表（EXE .data @0x4bb994 原样）。
inline constexpr i32 kLifePieceThresholds[8] = {8, 10, 12, 15, 18, 20, 25, 9999999};
inline i32 life_piece_threshold(i32 index) noexcept {
    return kLifePieceThresholds[index < 0 ? 0 : (index > 7 ? 7 : index)];
}

// ---- 灵界单一状态接口：DAT_004be808 值 + DAT_004be830 bit0x100 激活位 ----
// 单位 1/200 魂、上限 600。不要另建 HUD/敌吸附私有计量；显示层只读本值。
// 注意：以下 helper 只是原版写点的状态封装，尚未接入收集/输入/碰撞/死亡流程，
// 不构成完整激活/消耗状态机（采集、主动 trance、消耗仍属未集成）。
inline constexpr i32 kTranceSegment = 200;
inline constexpr i32 kTranceMax = 600;
inline constexpr i32 kTranceCost = 200;      // 0x405b80:78 正常结束扣 200
inline constexpr u32 kTranceActiveFlag = 0x100;  // DAT_004be830（SessionState.run_mode_flags）
// 只读访问器：唯一 backing 是 run_mode_flags bit0x100 与 ending_points。
inline bool trance_active(const SessionState& state) noexcept {
    return (state.run_mode_flags & kTranceActiveFlag) != 0;
}
inline i32 trance_value(const SessionState& state) noexcept { return state.ending_points; }
// 0x43d530：收集加值，仅上限 600（原版无下限钳制）。
inline void trance_add(SessionState& state, i32 amount) noexcept {
    const i32 value = state.ending_points + amount;
    state.ending_points = value > kTranceMax ? kTranceMax : value;
}
// 0x4057f0:15：主动激活门槛 = 满 600 且当前未激活（另需输入位 0x800 等）。
inline bool trance_ready(const SessionState& state) noexcept {
    return state.ending_points >= kTranceMax && !trance_active(state);
}
// 0x405a20：置激活位；param=1 的死亡保护另存于 StageBattle 生命周期
//（0x444a10：返回 0=启动成功 → state 1、不扣命；-1=失败 → 走普通死亡）。
inline void trance_begin(SessionState& state) noexcept {
    state.run_mode_flags |= kTranceActiveFlag;
}
// 0x405b80：清激活位；正常结束扣 200，死亡保护（state bit1=1）不扣。
inline void trance_end(SessionState& state, bool death_protection) noexcept {
    state.run_mode_flags &= ~kTranceActiveFlag;
    if (death_protection) return;
    state.ending_points -= kTranceCost;
    if (state.ending_points < 0) state.ending_points = 0;
}

// HUD 侧玩家资源视图（battle 未接入前的持有者；完整 PlayerLifeSession
// 随 Stage 分期落地，字段名与 th15 对齐便于迁移）。
// 图标碎片档位由原版公式决定：残机 (pieces*5)/threshold+7（0x42ae90:40，
// threshold=0x4bb994[be7fc]）、灵击 (pieces*6)/8+7（0x42af60:38）；script30/38
// 标签 2=满、3=空、7..12=碎片档，不是「5 碎片一整」。
struct HudPlayerState {
    i32 extra_lives = 2, life_pieces = 0;
    i32 bombs = 2, bomb_pieces = 0;
    i32 power = 100, max_power = 400, power_step = 100;
    i32 deaths = 0;
    u32 mode_flags = 0;
};

// HUD 分数视图：score 为逻辑分（尾位由 continues/hi_score_extra 补显示）。
struct HudScoreState {
    i32 score = 0;
    i32 life_piece_tier = 0;
};

}  // namespace th13
