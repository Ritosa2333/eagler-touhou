// GameHud 逻辑帧 pass（0x42c550/0x4265a0 boss 段接线：boss 视图非空即驱动）。
#include "GameHud.hpp"

namespace th13 {

namespace {
// 0x4265a0:85-96：ctx+0x180 bit0 是灵界槽“玩家左下靠近”淡化状态。
// 原版 FUN_0046fc30 把 label 写到 VM+0x4a0，并只向一层子链传播。
// 2026-11-02 恢复：round-19 重写此文件时状态机被误删（4fc0f57ce 起存在，
// 原版 42660c-42664a/42664c-42668e 反汇编复核：y>400(f64 0x4aee10) 且
// x<-64(0x4af068) → label5+置位；恢复 y<384(0x4aedb8) 或 x>-64 → label4+清位）。
constexpr u32 kTranceGaugeDimmed = 1u;
constexpr float kTranceGaugeEnterY = 400.f;  // 0x4aee10 = 400.0
constexpr float kTranceGaugeExitY = 384.f;   // 0x4aedb8 = 384.0
constexpr float kTranceGaugeEnterX = -64.f;  // 0x4af068 = -64.0
constexpr i32 kTranceGaugeDimLabel = 5;      // front 172-175: ins_409(10,0,32)
constexpr i32 kTranceGaugeRestoreLabel = 4;  // front 172-175: ins_409(30,0,255)
}  // namespace

bool GameHud::update(const HudFrameContext& context, HudFrameServices& services) {
    animations.rate = progress.rate;
    const auto sound = [&](i32 id) {
        if (services.hud_sound(id)) return true;
        error = "HUD sound service failed";
        return false;
    };
    (void)sound;  // 0x4265a0：帧侧无原版独立警告音调用点；Boss 段警告音已由 boss_hud.update 的 sound 回调（本文件 :55）处理。无原版独立调用点，BLOCKED——强行接线会多发声音，属行为回归。
    if (context.player_position) {
        // 0x4265a0:85-96：玩家进入左下角时灵界槽淡化（label 5 -> alpha 32），
        // 离开时恢复（label 4 -> alpha 255）。y 用 400/384 迟滞，x 用 -64。
        const float x = context.player_position->x;
        const float y = context.player_position->y;
        const bool dimmed = (flags & kTranceGaugeDimmed) != 0;
        if (!dimmed && y > kTranceGaugeEnterY && x < kTranceGaugeEnterX) {
            if (!animations.interrupt(gauge_main, kTranceGaugeDimLabel, false)) {
                if (error.empty())
                    error = animations.error.empty() ? "Trance gauge dim interrupt failed"
                                                     : animations.error;
                return false;
            }
            flags |= kTranceGaugeDimmed;
        } else if (dimmed && (y < kTranceGaugeExitY || x > kTranceGaugeEnterX)) {
            if (!animations.interrupt(gauge_main, kTranceGaugeRestoreLabel, false)) {
                if (error.empty())
                    error = animations.error.empty() ? "Trance gauge restore interrupt failed"
                                                     : animations.error;
                return false;
            }
            flags &= ~kTranceGaugeDimmed;
        }
    }
    // Boss 面（0x4265a0）已拆出为 update_boss：原版它是独立的 Update 0x1c=28
    // 任务（晚于道具 24/神霊 25），不随本聚合（RunScene 优先级 20）执行。
    // 开场签名幕不按战斗帧退休：原生 0x44c0d0 的卸载点在标题线程尾段
    // （TitleInf+0x5e4>=0xB4 或 DAT_004dcc5c&0x180），加速直入不跑标题线程，
    // sig 两半在原生 gate-01 全窗 battle-draws 每帧都在（0..3329，6660 条）。
    // 旧实现按 terra-udsr 5 帧短窗误读成「只画到 battle frame 3」并在
    // frame 4 退休，导致 boot-draw 窗口首差（frame 3 draw 118 sig 缺失）。
    frame_age.tick(&progress.rate);
    return true;
}

bool GameHud::update_boss(BossFrameInput& view, HudFrameServices& services) {
    // Boss 面独立推进（0x4265a0 同序：门控/计时警告/比值平滑/星位/邻近/位标）。
    // 2026-11-05 从 update() 拆出：原版任务优先级 0x1c=28（0x424f46/0x424f4b）。
    if (!boss_hud.update(view,
                         [&services](i32 id) { return services.hud_sound(id); }) ||
        !boss_hud.error.empty()) {
        if (error.empty()) error = boss_hud.error.empty() ? "Boss HUD update failed"
                                                          : boss_hud.error;
        return false;
    }
    return true;
}

}  // namespace th13
