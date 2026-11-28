#pragma once

#include "HudFrameData.hpp"

#include "SessionState.hpp"

#include <array>
#include <string>

namespace th13 {

struct HudTextStyle {
    u32 color = 0xffffffff;
    Vec2 scale{1, 1};
    i32 font = 0, coordinate_space = 0, alignment = 1, vertical_alignment = 1;
    bool shadow = false;
};

struct HudTextDraw {
    Vec3 position{};
    HudTextStyle style;
    std::string text;
};

struct MpHudSeats {
    i32 count = 0;
    i32 local_seat = 0;
    u8 ghost = 0;
    bool challenge = false;
    std::array<const HudPlayerState*, 3> seats{};
};

struct HudDrawServices {
    virtual ~HudDrawServices() = default;
    virtual bool hud_text(const HudTextDraw&) = 0;
};

struct HudDrawContext {
    HudFrameContext frame;
    const MpHudSeats* mp = nullptr;
};

std::string grouped_number(u32 value);
std::string score_number(u32 value, i32 last_digit);

}
