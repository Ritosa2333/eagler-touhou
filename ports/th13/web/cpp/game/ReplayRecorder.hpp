#pragma once
#include "GameInput.hpp"
#include "ReplayFile.hpp"
#include "SessionState.hpp"

namespace th13 {
// Collects the original six-byte input records and emits a t13r container.
// The caller supplies the exact 0x74 and 0xc4 start snapshots.
class ReplayRecorder {
public:
    bool start(const std::vector<u8>& top);
    bool begin_stage(const std::vector<u8>& header);
    bool record(const GameInput& input, u8 slow_rate);
    bool encode(std::vector<u8>& out, const std::string& name, i32 score) const;
    static std::vector<u8> top_snapshot(const SessionState& progress);
    static std::vector<u8> stage_snapshot(const SessionState& progress,
        const HudPlayerState& player, const HudScoreState& score, u16 seed,
        i32 x128, i32 y128, bool focused);
    bool ready() const { return top_.size() == 0x74 && !stages_.empty(); }
    void clear() { top_.clear(); stages_.clear(); }
private:
    std::vector<u8> top_;
    std::vector<ReplayStage> stages_;
};
}
