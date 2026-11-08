// HUD 文本绘制通道（Frame/Draw 分离的 Draw 侧；与 th15 同构，
// 经 ApplicationState::hud_text 单通道入 AsciiText 队列）。
#pragma once

#include "HudFrameData.hpp"

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

struct HudDrawServices {
    virtual ~HudDrawServices() = default;
    virtual bool hud_text(const HudTextDraw&) = 0;
};

struct HudDrawContext {
    HudFrameContext frame;
};

// 千分位分组与带尾位 score 格式（th15 GameHudDraw.cpp 同族实现）。
std::string grouped_number(u32 value);
std::string score_number(u32 value, i32 last_digit);

}  // namespace th13
