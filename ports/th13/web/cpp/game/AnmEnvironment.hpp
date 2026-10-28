#pragma once
#include "GameRandom.hpp"
#include "Types.hpp"
#include <array>
namespace th13 {
class AnmResource;
// Scene-owned values and the game RNG are supplied by the game. No global memory
// addresses or graphics API handles are visible to animation instructions.
struct AnmEnvironment {
    GameRandom game_rng;
    Vec3 camera_origin{},screen_translation{},reference_position{};
    AnmResource* fallback_sprite_resource=nullptr;
    float resolution_scale=1;std::array<i32,4> screen_offsets{};
    // Physical pixel grid; logical game coordinates and Replay stay unchanged.
    float raster_scale=1;
    Vec3 background_delta{};bool paused=false;
    Vec2 screen_shake_offset{}; // 4dcbe8/4dcbec, copied to the STD camera by406d90/407310
    Vec3 stage_direction{}; // raw camera direction dcb00; reference_position is normalized dcb18
    i32 stage_difficulty=0; // original be824, extra depth displacement above Easy
    u32 screen_width=640,screen_height=480;
    // Overlay masking follows the screen alpha format, not a menu mode.
    bool render_target_has_alpha=false;
    // Original logical shot clipping rectangle (51bc08/51bc0c), distinct
    // from the centered sprite offsets used by ANM coordinates.
    Vec2 playfield_origin{};
};
}
