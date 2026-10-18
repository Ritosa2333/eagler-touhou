#pragma once
#include "EnemyState.hpp"
namespace th15 {
struct HudFrameContext {
 const EnemyWorldState* enemies=nullptr;const Vec3* player_position=nullptr;
 u32 spell_flags=0;bool dialogue=false;
 std::array<i32,2> boss_banners{{-1,-1}};
};
struct HudFrameServices {virtual ~HudFrameServices()=default;virtual bool hud_sound(i32)=0;};
struct HudBossAnimations {std::array<u32,7> handles{};i32 created=0,dimmed=0;};
struct HudResultCounter {float remaining=0,initial=0;i32 total=0,current=0,increment=0,limit=0;};
}
