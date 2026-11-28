#pragma once

#include "BossPresentation.hpp"
#include "Types.hpp"

namespace th13 {

struct HudFrameContext {
    const Vec3* player_position = nullptr;
    bool dialogue = false;
    const BossFrameInput* boss = nullptr;
};

struct HudFrameServices {
    virtual ~HudFrameServices() = default;
    virtual bool hud_sound(i32) = 0;
};

}
