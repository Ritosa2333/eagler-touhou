#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace th13::multiplayer {

struct SessionSetup {
    static constexpr std::uint32_t Version = 2, LegacyVersion = 1, Words = 22,
                                  GameplayVersion = 5, MaxRollbackFrames = 12,
                                  RulesVersion = 3;
    static constexpr std::uint32_t MaxCharacters = 4;
    std::uint32_t version = LegacyVersion, mode = 1;
    std::uint64_t session_id = 0;
    std::uint32_t player_count = 2, local_player = 0, difficulty = 0, seed = 1;
    std::uint32_t requested_delay = 0, input_delay = 0, prediction_reserve = 2;
    std::uint32_t prediction_limit = MaxRollbackFrames, measured_prediction = 0;
    bool automatic = true, challenge = false, keep_bullets_on_miss = false;
    std::array<std::uint32_t, 3> selections{};
    std::array<std::uint32_t, 4> build{};
    bool valid() const;
};

bool DecodeSessionSetup(SessionSetup&, const std::uint32_t*, std::size_t);
std::array<std::uint32_t, SessionSetup::Words> EncodeSessionSetup(const SessionSetup&);
std::uint32_t GameplayAbi(const SessionSetup&);

}
