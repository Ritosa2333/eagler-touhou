#pragma once
#include "Rng.hpp"
#include "Types.hpp"
#include <array>
namespace th15 {
class AnmResource;
// Scene-owned values and the game RNG are supplied by the game. No global memory
// addresses or graphics API handles are visible to animation instructions.
struct AnmEnvironment {
    Rng game_rng;
    Vec3 camera_origin{},screen_translation{},reference_position{};
    AnmResource* fallback_sprite_resource=nullptr;
    float resolution_scale=1;std::array<i32,4> screen_offsets{};
    // Physical pixel grid; logical game coordinates and Replay stay unchanged.
    float raster_scale=1;
    Vec3 background_delta{};bool paused=false;
    u32 screen_width=640,screen_height=480;
    // Overlay masking follows the screen alpha format, not a menu mode.
    bool render_target_has_alpha=false;
    // Original logical shot clipping rectangle (51bc08/51bc0c), distinct
    // from the centered sprite offsets used by ANM coordinates.
    Vec2 playfield_origin{};
};
}
