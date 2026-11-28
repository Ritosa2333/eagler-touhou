#pragma once
#include "GameInput.hpp"
#include "ReplayFile.hpp"
#include "SessionState.hpp"
#include <ctime>

namespace th13 {

struct ReplaySaveMeta {
    std::time_t date = 0;
    double slow_numerator = 0.0;
    double slow_denominator = 0.0;
    i32 stage_field = 8;
    bool append_eof = false;
    const char* version = "1.00c";
};

class ReplayRecorder {
public:
    bool start(const std::vector<u8>& top);
    bool begin_stage(const std::vector<u8>& header);
    bool record(const GameInput& input, u8 slow_rate);
    bool record_spell_time(u32 index, i32 packed);
    bool encode(std::vector<u8>& out, const std::string& name, i32 score) const;
    bool encode(std::vector<u8>& out, const std::string& name, i32 score, const ReplaySaveMeta& meta) const;
    static std::vector<u8> top_snapshot(const SessionState& progress);
    static std::vector<u8> stage_snapshot(const SessionState& progress, const HudPlayerState& player,
                                          const HudScoreState& score, u16 seed, i32 x128, i32 y128, bool focused);
    bool ready() const { return top_.size() == 0x74 && !stages_.empty(); }
    void clear() {
        top_.clear();
        stages_.clear();
    }

private:
    std::vector<u8> top_;
    std::vector<ReplayStage> stages_;
};
}
