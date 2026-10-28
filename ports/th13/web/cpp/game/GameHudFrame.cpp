// GameHud 逻辑帧 pass（0x42c550/0x4265a0 boss 段接线：boss 视图非空即驱动）。
#include "GameHud.hpp"

namespace th13 {

namespace {
// 0x4265a0:85-96：ctx+0x180 bit0 是灵界槽“玩家左下靠近”淡化状态。
// 原版 FUN_0046fc30 把 label 写到 VM+0x4a0，并只向一层子链传播。
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
    (void)sound;  // 通知/符卡音效随对应分期接入
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
    // Boss 面（0x4265a0 同序：门控/计时警告/比值平滑/星位/邻近/位标）。
    if (context.boss) {
        if (!boss_hud.update(*context.boss,
                             [&services](i32 id) { return services.hud_sound(id); }) ||
            !boss_hud.error.empty()) {
            if (error.empty()) error = boss_hud.error.empty() ? "Boss HUD update failed"
                                                              : boss_hud.error;
            return false;
        }
    }
    frame_age.tick(&progress.rate);
    return true;
}

}  // namespace th13
