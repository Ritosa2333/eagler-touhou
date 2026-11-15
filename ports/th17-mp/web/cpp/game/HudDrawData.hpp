#pragma once
#include "Types.hpp"
#include "ItemCollection.hpp"
#include <string>
namespace th17 {
struct HudTextStyle {u32 color=0xffffffff;Vec2 scale{1,1};i32 font=0,coordinate_space=0,alignment=1,vertical_alignment=1;bool shadow=false;};
struct HudTextDraw {Vec3 position{};HudTextStyle style;std::string text;};
struct HudDrawServices {virtual ~HudDrawServices()=default;virtual bool hud_text(const HudTextDraw&)=0;};
struct HudScoreState {i32 high_score=0,high_score_extra=0,continues=0;u32 mode_flags=0;};
struct HudDrawContext {
    const ItemScoreState& score;const HudScoreState& progress;
    i32 graze_total=0;bool spell_available=false;i32 spell_frames=0,spell_best_time=0;
};
std::string grouped_number(u32 value);std::string score_number(u32 value,i32 last_digit);
}
