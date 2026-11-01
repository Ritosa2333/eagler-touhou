// GameHud 逻辑帧 pass（0x42c550/0x4265a0 boss 段接线：boss 视图非空即驱动）。
#include "GameHud.hpp"

namespace th13 {

bool GameHud::update(const HudFrameContext& context, HudFrameServices& services) {
    animations.rate = progress.rate;
    const auto sound = [&](i32 id) {
        if (services.hud_sound(id)) return true;
        error = "HUD sound service failed";
        return false;
    };
    (void)sound;  // 通知/符卡音效随对应分期接入
    if (context.player_position) {
        // th15 0x200 标志（玩家贴上沿隐藏 boss 部件）随 boss HUD 分期。
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
