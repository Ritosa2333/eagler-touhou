#pragma once

#include "ReplayFile.hpp"
#include "Rng.hpp"
#include "SessionState.hpp"

#include <cstdio>

namespace th13 {
class StageBattle;
enum class ReplayRestorePhase { Load, Transition };

struct ReplayRestoreTarget {
    SessionState* progress = nullptr;
    HudPlayerState* player = nullptr;
    HudScoreState* score = nullptr;
    GameRandom* game_random = nullptr;
    GameRandom* visual_random = nullptr;
};

struct ReplayRestoreLog {
    u16 seed = 0;
    u32 stage_number = 0;
    i32 block[22] = {};
    static constexpr u32 kUnmappedOffsets[5] = {0x18, 0x28, 0x34, 0x3C, 0x68};
};

class ReplaySession {
public:
    static bool restore_player_start(const ReplayStage&, StageBattle&, std::string& error);
    static i32 stage_index(const ReplayFile& file, i32 stage_number);
    static bool restore_stage(const ReplayFile& file, u32 stage_index, const ReplayRestoreTarget& target,
                              ReplayRestoreLog& log, std::string& error,
                              ReplayRestorePhase phase = ReplayRestorePhase::Transition);
    static void emit_log(const ReplayRestoreLog& log) {
        std::fprintf(stderr, "R3-RESTORE stage=%u seed=%u counter=0 block=", log.stage_number, unsigned(log.seed));
        for (u32 k = 0; k < 22; ++k)
            std::fprintf(stderr, "%s%d", k ? "," : "", log.block[k]);
        std::fprintf(stderr, "\n");
    }
};

}
