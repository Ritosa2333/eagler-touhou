#pragma once

#include "../game/Types.hpp"

namespace th13::sdl {

inline constexpr u32 kStageLoaderSceneFlag = 4u;

inline u32 stage_loader_completed_scene_flags(u32 scene_flags) noexcept { return scene_flags & ~kStageLoaderSceneFlag; }

inline bool stage_loading_holds_game_fade(i32 kind, u32 scene_flags) noexcept {
    return kind == 5 && (scene_flags & 5u) != 0;
}

inline constexpr i32 kRunPointInitTable[6] = {5000, 10000, 15000, 20000, 20000, 100000};
inline constexpr i32 kRunPointCapTable[6] = {100000, 250000, 500000, 500000, 500000, 1000000};

inline void run_entry_reset_points(bool continuing, i32 difficulty, i32& score, i32& max_point, i32& max_point_cap,
                                   i32& graze) noexcept {
    if (continuing) return;
    score = 0;
    max_point = kRunPointInitTable[difficulty] * 100;
    max_point_cap = kRunPointCapTable[difficulty] * 100;
    graze = 0;
}

}
