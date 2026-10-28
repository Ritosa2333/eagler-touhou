#include "PostGameFlow.hpp"

#include "ScoreBoard.hpp"

namespace th13 {

void PostGameFlow::enter(OverlayState next) {
    state_ = next;
    age_ = 0;
    // 三族入场共语义：置 0x10 冻结位（0x42c550:263 三旗 0x10/0x20/0x40 任一
    // 非零 → 对局 update 返 3 不推帧；移植 = SessionState.scene_flags）。
    progress_.scene_flags |= 0x10u;
}

void PostGameFlow::apply_stage_clear_bonus() {
    // 0x42abf0：bonus = 当前关号 × 1,000,000（递增前的 DAT_004be81c）；
    // +0x2ac 覆盖写、be7c0 += bonus/10 封顶；+0x180|=0x100（HUD 通关演出）
    // 与 stagectx+0x110 fade VM 的视觉面留 TODO（结算窗 oracle 一起锚定）。
    const i32 bonus = stage_clear_bonus(progress_.stage);
    progress_.stage_clear_bonus = bonus;
    score_.score = i32(add_bonus_capped(u32(score_.score), bonus));
    // 通关 HUD 闪烁位（+0x180 bit8）——HUD 侧消费 TODO。
}

void PostGameFlow::on_stage_clear() {
    // 0x42cd40 直译。mode = DAT_004be830>>4 & 3；符卡练习（&0x30==0x20）
    // 不吃每关百万奖。
    const u32 mode = (progress_.run_mode_flags >> 4) & 3;
    const bool spell_practice = (progress_.run_mode_flags & 0x30u) == 0x20u;
    if (!spell_practice) apply_stage_clear_bonus();
    const bool practice = progress_.practice_mode != 0;  // GameInf+0x74
    if (mode == 0) {
        if (progress_.stage == 6) {
            // 六面：通关演出（0x4000）+ 好结局资格 + 全通奖四式（0x42cd40:27-58）。
            progress_.scene_flags |= 0x4000u;  // GameInf+0x60
            clear_sequence_ = true;
            clear_frame_ = 0;
            if (progress_.ending_points > 599) progress_.good_endings++;
            if (player_.bombs > 2) progress_.good_endings = 3;
            const i32 bonus = all_clear_bonus(progress_.difficulty, player_.extra_lives,
                                              player_.bombs);
            score_.score = i32(add_bonus_capped(u32(score_.score), bonus));
            progress_.stage_clear_bonus += bonus;  // +0x2ac 累计
            // 通关次数 / 无续关通关计数（score 块 +0x7c8/+0x7e4，练习不记）
            // —— ScoreFile 落档期 TODO。
            // 42cd40 checks GameInf+74 immediately after awarding the bonus.
            // Replay/practice opens43e5a0 here; its0x10 freeze also suspends
            // the0x4000 clear sequence. Only ordinary play waits for Ending.
            if (practice) on_practice_end();
            return;
        }
        if (progress_.stage == 7) {
            // Extra：0x180|=0x10 + Extra 奖 + 结算B（0x42cd40:73-96）。
            const i32 bonus = extra_clear_bonus(player_.extra_lives, player_.bombs);
            score_.score = i32(add_bonus_capped(u32(score_.score), bonus));
            progress_.stage_clear_bonus += bonus;
            if (practice) {
                on_practice_end();
            } else {
                on_extra_end();  // 0x43f720
            }
            return;
        }
        // 关 1-5 主干：dcc24=0xc 无条件；be81c++（<7）；stage 表指针推进。
        if (!practice) {
            // score+0x7fc 起关卡通过位（0x42cd40:97-99；内存态，落档 TODO）。
            const u32 bit = u32(progress_.stage + progress_.difficulty * 8);
            if (bit < 32) progress_.stage_cleared_bits |= 1u << bit;
        }
        host_.request_destination(0xc);  // DAT_004dcc24 = 0xc
        if (progress_.stage < 7) progress_.stage++;
        // stage 参数表指针（DAT_004c22dc = 0x4bb4f0+stage*0x7c）与下一关资产
        // 装载——Application.begin_run 多关化接线 TODO（本类不直接装载）。
        return;
    }
    // mode 1（Extra）/2（符卡练习）：小分落档（be7d4 槽，ScoreFile TODO）+
    // 结算B；练习参数非 0 改走结算A（0x42cd40:110-125）。
    if (practice) {
        on_practice_end();
    } else {
        on_extra_end();
    }
}

void PostGameFlow::on_game_over() {
    // 0x43f570 骨架：mode0 → 态 0xe（GameOver 展示）；mode≠0 → 0x18（结算B
    // GameOver 变体）；置 0x10、BGM th128_08、slow rate 记录 TODO。
    host_.bgm(7);  // 「th128_08」（0x43f570:58；音频接线 TODO，先记号）
    const u32 mode = (progress_.run_mode_flags >> 4) & 3;
    if (mode == 0) {
        enter(OverlayState::GameOverShow);
        // The retail menu enables Continue only while DAT_004be844 > 0.
        // Keep the decision source in session state for the pause host.
        if (progress_.continue_stock < 0) progress_.continue_stock = 0;
        // 后续：0x43d650 落分 stage ctx+0x144/刷新 be810 → 0x43f8f0 排名门
        // → +0x1f8==0 态 0x13 否则态 0xf+建脚本 142。菜单交互/续关执行
        //（态 0x15：残2/灵2/灵力0/白点200 重置、be7c8++/be844--）TODO。
    } else {
        enter(OverlayState::BGameOver);
        // 0x440960 态 0x18 族菜单交互 TODO。
    }
}

bool PostGameFlow::on_practice_end() {
    // 0x43e5a0 骨架：态 2（练习结算入场）+ 结算页变体 1（DAT_004df120）。
    enter(OverlayState::PracticeEntry);
    if (!host_.request_practice_end()) {
        if (error.empty()) error = "Practice end request failed";
        return false;
    }
    return true;
}

void PostGameFlow::on_extra_end() {
    // 0x43f720 入口分流（anatomy §0/§1.4）：mode2 → 0x1e（符卡练习变体）、
    // mode≠0 → 0x1f（Extra/符卡通关）、mode0 → 0x17（Extra 通关后的展示，
    // stage7 关终走此路——be830 保持 mode0）。
    const u32 mode = (progress_.run_mode_flags >> 4) & 3;
    if (mode == 2) {
        enter(OverlayState::BSpellVariant);
    } else if (mode == 0) {
        enter(OverlayState::BShow);  // 0x43d650 落分/BGM th128_08/计速 TODO
    } else {
        enter(OverlayState::BClear);  // 0x1f：+0x50 后 0x441930 + 0x43d820
                                      // small score 落档 TODO
    }
    if (!host_.request_extra_results()) error = "Extra results request failed";
}

void PostGameFlow::update() {
    age_++;
    if (clear_sequence_) {
        // 0x42c550:20-57（GameInf 0x4000 路径）：覆盖层 0x10 激活时不推帧
        //（返 3）；+0x78 计数 == 0xb4(180) 建淡出任务（视觉 TODO）；
        // > 0x17b(379) 出口——练习 → 0x43e5a0，常规 → 请求 0xf 结局。
        if (frozen()) return;
        clear_frame_++;
        if (clear_frame_ == 180) {
            if (!host_.request_stage6_fade()) {
                error = "Stage 6 clear fade request failed";
                return;
            }
        }
        if (clear_frame_ > 379) {
            clear_sequence_ = false;
            progress_.scene_flags &= ~0x4000u;
            if (progress_.practice_mode != 0) {
                on_practice_end();
            } else {
                host_.request_destination(0xf);  // 结局（0x42c550:52-57 的
                // 非激活降级 2 属平台侧护栏 TODO，见 0x414fc0）
            }
        }
        return;
    }
    // 结算A/GameOver/结算B 各态的逐帧处理器（0x43e780/0x43fa90/0x440960）
    // ——菜单导航、名前录入、回放保存：下一阶段随结算窗 oracle 落地。
}

}  // namespace th13
