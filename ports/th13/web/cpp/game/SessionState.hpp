#pragma once

#include "Types.hpp"

namespace th13 {

struct SessionState {
    i32 stage = 1, stage_frame = 0, character = 0;
    i32 difficulty = 0, continues = 0;
    i32 continue_stock = 3;
    i32 high_score = 100000, high_score_extra = 0;
    i32 configured_lives = 3;
    i32 starting_stage = 1;
    i32 chapter = 0, chapter_frame = 0;
    float rate = 1;
    u32 scene_flags = 0;
    bool new_run = true;
    u32 run_mode_flags = 0;
    i32 practice_mode = 0;
    i32 selected_spell_id = -1;
    i32 ending_points = 0;
    i32 good_endings = 0;
    i32 stage_clear_bonus = 0;
    u32 stage_cleared_bits = 0;
    i32 point_items = 0;
    i32 max_point = 0, max_point_cap = 0;
    i32 graze = 0;
    i32 life_piece_threshold_index = 0;
};

inline u8 ecl_difficulty_mask(i32 difficulty) noexcept {
    return difficulty >= 0 && difficulty < 6 ? u8(1u << difficulty) : 0;
}

inline bool records_practice_stage(const SessionState& state, bool replay) noexcept {
    return !replay && state.practice_mode == 0 && (state.run_mode_flags & 0x70u) == 0 && state.character >= 0 &&
           state.character < 4 && state.difficulty >= 0 && state.difficulty < 4 && state.stage >= 1 && state.stage <= 6;
}

inline bool returns_to_practice_selection(const SessionState& state, i32 reason, bool replay) noexcept {
    return (reason == 1 || reason == 3) && !replay && state.practice_mode != 0 &&
           (state.run_mode_flags & 0x70u) == 0x10u && state.character >= 0 && state.character < 4 &&
           state.difficulty >= 0 && state.difficulty < 4 && state.starting_stage >= 1 && state.starting_stage <= 6;
}

inline bool returns_to_spell_practice_selection(const SessionState& state, i32 reason, bool replay) noexcept {
    return (reason == 1 || reason == 3) && !replay && state.practice_mode == 0 &&
           (state.run_mode_flags & 0x70u) == 0x20u && state.character >= 0 && state.character < 4 &&
           state.starting_stage >= 1 && state.starting_stage <= 7 && state.selected_spell_id >= 0;
}

inline constexpr i32 kLifePieceThresholds[8] = {8, 10, 12, 15, 18, 20, 25, 9999999};
inline i32 life_piece_threshold(i32 index) noexcept {
    return kLifePieceThresholds[index < 0 ? 0 : (index > 7 ? 7 : index)];
}

inline constexpr i32 kTranceSegment = 200;
inline constexpr i32 kTranceMax = 600;
inline constexpr i32 kTranceCost = 200;
inline constexpr u32 kTranceActiveFlag = 0x100;
inline bool trance_active(const SessionState& state) noexcept {
    return (state.run_mode_flags & kTranceActiveFlag) != 0;
}
inline i32 trance_value(const SessionState& state) noexcept { return state.ending_points; }
inline void trance_add(SessionState& state, i32 amount) noexcept {
    const i32 value = state.ending_points + amount;
    state.ending_points = value > kTranceMax ? kTranceMax : value;
}
inline bool trance_ready(const SessionState& state) noexcept {
    return state.ending_points >= kTranceMax && !trance_active(state);
}
inline void trance_begin(SessionState& state) noexcept { state.run_mode_flags |= kTranceActiveFlag; }
inline void trance_end(SessionState& state, bool death_protection) noexcept {
    state.run_mode_flags &= ~kTranceActiveFlag;
    if (death_protection) return;
    state.ending_points -= kTranceCost;
    if (state.ending_points < 0) state.ending_points = 0;
}
inline void trance_reset_fresh_entry(SessionState& state) noexcept {
    state.ending_points = (state.run_mode_flags & 0x30u) == 0x20u ? 0 : kTranceSegment;
}

struct HudPlayerState {
    i32 extra_lives = 2, life_pieces = 0;
    i32 bombs = 2, bomb_pieces = 0;
    i32 power = 100, max_power = 400, power_step = 100;
    i32 deaths = 0;
    u32 mode_flags = 0;
};

struct HudScoreState {
    i32 score = 0;
    i32 life_piece_tier = 0;
};

}
