// HUD 逻辑帧上下文与纯回调服务（Frame/Draw 分离的 Frame 侧；
// 裁剪自 th15：敌方世界/boss/符卡/chapter 通知延后，docs/plans/ui-base.md §三）。
#pragma once

#include "BossPresentation.hpp"
#include "Types.hpp"

namespace th13 {

struct HudFrameContext {
    const Vec3* player_position = nullptr;
    bool dialogue = false;
    // Boss/符卡面每帧视图（RunScene 装配；null = 本帧无 boss 面）。
    const BossFrameInput* boss = nullptr;
};

struct HudFrameServices {
    virtual ~HudFrameServices() = default;
    virtual bool hud_sound(i32) = 0;
};

}  // namespace th13
