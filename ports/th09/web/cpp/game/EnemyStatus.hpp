#pragma once
#include "Timer.hpp"
namespace th09 {
struct EclVm;struct EclInstruction;
struct EnemyStatus {
    i32 initial_life=0,maximum_life=0,life_subroutines[4]{},timeout=0,timeout_subroutine=0;
    i16 death_subroutine=0;u8 draw_group=0;i8 effects[3]{};
    Timer invulnerable{0,0,0};i32 attached_effect_count=0;float attached_effect_radius=0;
    i32 index=0;u32 saved_color=0;
    i32 shot_damage[2]{},remaining_seconds=0;u8 damage_flash=0;
    // Preserved ECL motion settings: the original initializes and writes this
    // range and four modes even though the analyzed update never reads them.
    Vec2 motion_range;i16 motion_modes[4]{};
    bool command(EclVm&,EclInstruction&);
};
}
