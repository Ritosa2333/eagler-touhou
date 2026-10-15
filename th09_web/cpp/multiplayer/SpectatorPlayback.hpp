#pragma once
#include <algorithm>
#include <cstdint>

namespace th09::multiplayer {
// Viewer-only cursor policy. Never predicts, rewinds, or changes player pacing.
struct SpectatorPlayback {
    static constexpr std::uint32_t Delay = 90;
    static constexpr std::uint32_t Capacity = 32400;
    bool paused = false, following = false, seeking = false, ended = false;
    std::uint32_t target = 0, roundStart = 0, round = 0;
    void Boundary(std::uint32_t frame, std::uint32_t nextRound) {
        roundStart = frame; round = nextRound;
        // A forward target can cross rounds within the same match.
        if (!seeking || target <= frame) { target = frame; seeking = false; }
    }
    static std::uint32_t Edge(std::uint32_t received) {
        return received > Delay ? received - Delay : 0;
    }
    std::uint32_t Limit(std::uint32_t received) const { return ended ? received : Edge(received); }
    bool Control(unsigned action, std::uint32_t to, std::uint32_t played, std::uint32_t received) {
        if(action > 3) return false;
        if(action == 0) { paused = true; return true; }
        if(action == 3) { paused = false; return true; }
        if(action == 1 && (to < played || to > Limit(received))) return false;
        if(action == 2 && to > played && to - played > Capacity) return false;
        paused = false; following = action == 2;
        // JS may still be feeding already-received history in bounded batches.
        // Available() still limits every tick to native-validated input.
        target = following ? std::max(played, Limit(std::max(received,to))) : to;
        seeking = target > played;
        return true;
    }
    bool Fast(std::uint32_t played, std::uint32_t received) {
        // Live is a one-shot seek. Later network bursts must not move its
        // target or re-arm acceleration after ordinary playback resumes.
        (void)received;
        if(played >= target) seeking = false;
        return seeking;
    }
    std::uint32_t Available(std::uint32_t played, std::uint32_t received) const {
        if(paused || Limit(received) <= played) return 0;
        return std::min(Limit(received) - played,
            seeking ? target > played ? target - played : 0 : Limit(received) - played);
    }
};
}
