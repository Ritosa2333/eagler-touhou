#include "GameHud.hpp"

namespace th13 {

namespace {
constexpr u32 kTranceGaugeDimmed = 1u;
constexpr float kTranceGaugeEnterY = 400.f;
constexpr float kTranceGaugeExitY = 384.f;
constexpr float kTranceGaugeEnterX = -64.f;
constexpr i32 kTranceGaugeDimLabel = 5;
constexpr i32 kTranceGaugeRestoreLabel = 4;
}

bool GameHud::update(const HudFrameContext& context, HudFrameServices& services) {
    animations.rate = progress.rate;
    const auto sound = [&](i32 id) {
        if (services.hud_sound(id)) return true;
        error = "HUD sound service failed";
        return false;
    };
    (void)sound;
    if (context.player_position) {
        const float x = context.player_position->x;
        const float y = context.player_position->y;
        const bool dimmed = (flags & kTranceGaugeDimmed) != 0;
        if (!dimmed && y > kTranceGaugeEnterY && x < kTranceGaugeEnterX) {
            if (!animations.interrupt(gauge_main, kTranceGaugeDimLabel, false)) {
                if (error.empty())
                    error = animations.error.empty() ? "Trance gauge dim interrupt failed" : animations.error;
                return false;
            }
            flags |= kTranceGaugeDimmed;
        } else if (dimmed && (y < kTranceGaugeExitY || x > kTranceGaugeEnterX)) {
            if (!animations.interrupt(gauge_main, kTranceGaugeRestoreLabel, false)) {
                if (error.empty())
                    error = animations.error.empty() ? "Trance gauge restore interrupt failed" : animations.error;
                return false;
            }
            flags &= ~kTranceGaugeDimmed;
        }
    }
    frame_age.tick(&progress.rate);
    return true;
}

bool GameHud::update_boss(BossFrameInput& view, HudFrameServices& services) {
    if (!boss_hud.update(view, [&services](i32 id) { return services.hud_sound(id); }) || !boss_hud.error.empty()) {
        if (error.empty()) error = boss_hud.error.empty() ? "Boss HUD update failed" : boss_hud.error;
        return false;
    }
    return true;
}

}
