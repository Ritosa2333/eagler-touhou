#pragma once
#include "GameRandom.hpp"
#include "Types.hpp"
#include <array>
namespace th13 {
class AnmResource;
struct AnmEnvironment {
    GameRandom game_rng;
    GameRandom* shared_game_rng = nullptr;
    GameRandom& distortion_random() noexcept { return shared_game_rng ? *shared_game_rng : game_rng; }
    Vec3 camera_origin{}, screen_translation{}, reference_position{};
    AnmResource* fallback_sprite_resource = nullptr;
    float resolution_scale = 1;
    std::array<i32, 4> screen_offsets{};
    float raster_scale = 1;
    Vec3 background_delta{};
    bool paused = false;
    Vec2 screen_shake_offset{};
    Vec3 stage_direction{};
    i32 stage_difficulty = 0;
    const i32* chapter = nullptr;
    u32 screen_width = 640, screen_height = 480;
    bool render_target_has_alpha = false;
    Vec2 playfield_origin{};
};
inline bool seed_native_camera_block_if_cleared(AnmEnvironment& environment) noexcept {
    if (environment.screen_translation.x != 0.f || environment.screen_translation.y != 0.f ||
        environment.screen_translation.z != 0.f)
        return false;
    environment.camera_origin = {};
    environment.screen_translation = {0.f, 0.f, 1000.f};
    environment.reference_position = {};
    environment.stage_direction = {};
    return true;
}
}
