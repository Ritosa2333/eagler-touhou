#pragma once

#include "../game/GameInput.hpp"
#include "../game/SessionState.hpp"

#include <array>
#include <cstdint>
#include <optional>
#include <vector>

namespace th13 {
class StageBattle;
}

namespace th13::multiplayer {

struct StageEntry {
    static constexpr std::uint32_t Version = 2, LegacyVersion = 1;
    static constexpr std::uint32_t MaxSeats = 3;

    std::uint32_t stage = 1;
    std::uint32_t chapter_frame = 0;
    std::int32_t ending_points = 0;
    std::int32_t good_endings = 0;
    std::int32_t point_items = 0;
    std::int32_t max_point = 0, max_point_cap = 0;
    std::int32_t graze = 0;
    std::int32_t life_piece_threshold_index = 0;
    std::int32_t score = 0;
    std::int32_t high_score = 0;
    struct Seat {
        std::int32_t extra_lives = 2, life_pieces = 0;
        std::int32_t life_piece_threshold_index = 0;
        std::int32_t bombs = 2, bomb_pieces = 0;
        std::int32_t power = 100, max_power = 400, power_step = 100;
        std::int32_t deaths = 0;
        std::uint32_t mode_flags = 0;
        std::int32_t character = 0;
        bool ghost = false;
    };
    std::array<Seat, MaxSeats> seats{};
    std::uint32_t seat_count = 0;
    std::int32_t trance_mode = 0;
    std::int32_t trance_owner_seat = 0;
    std::uint32_t trance_generation = 0;
    std::int32_t trance_task_frame = 0;
    std::uint16_t game_seed = 0;
    std::uint32_t game_index = 0;
    std::uint16_t visual_seed = 0;
    std::uint32_t visual_index = 0;
    std::array<GameInput, MaxSeats> inputs{};

    bool valid() const {
        return stage >= 1 && stage <= 7 && seat_count >= 1 && seat_count <= MaxSeats;
    }

    std::vector<std::uint8_t> Encode() const;
    static bool Decode(const std::uint8_t* bytes, std::size_t size, StageEntry& out);
    void Capture(const th13::StageBattle& battle, const SessionState& progress,
                 const HudScoreState& score, std::uint16_t game_seed,
                 std::uint32_t game_index, std::uint16_t visual_seed,
                 std::uint32_t visual_index);
    void Restore(th13::StageBattle& battle, SessionState& progress,
                 HudScoreState& score, std::uint16_t& game_seed, std::uint32_t& game_index,
                 std::uint16_t& visual_seed, std::uint32_t& visual_index) const;
    void PrepareStageClear(HudPlayerState& first_player, const th13::StageBattle& previous);
    void RestoreBattleCarry(th13::StageBattle& battle) const;
    void RestoreStageClearPositions(th13::StageBattle& battle) const;

private:
    std::array<std::optional<std::array<std::int32_t, 2>>, MaxSeats> stage_clear_positions_{};
};

}
