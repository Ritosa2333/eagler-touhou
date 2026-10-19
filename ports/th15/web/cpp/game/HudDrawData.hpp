#pragma once
#include "HudFrameData.hpp"
namespace th15 {
struct HudTextStyle {u32 color=0xffffffff;Vec2 scale{1,1};i32 font=0,coordinate_space=0,alignment=1,vertical_alignment=1;bool shadow=false;};
struct HudTextDraw {Vec3 position{};HudTextStyle style;std::string text;};
struct HudDrawServices {virtual ~HudDrawServices()=default;virtual bool hud_text(const HudTextDraw&)=0;};
struct HudDrawContext {HudFrameContext frame;bool spell_available=false;i32 spell_frames=0,spell_best_time=0;};
std::string grouped_number(u32 value);std::string score_number(u32 value,i32 last_digit);
}
