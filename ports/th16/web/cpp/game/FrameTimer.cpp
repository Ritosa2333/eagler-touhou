#include "FrameTimer.hpp"

namespace th16 {

void FrameTimer::reset() {
    origin = 0;
    over65_streak = 0;
    window_frames = 0;
    elapsed_good = 0;
    elapsed_total = 0;
    fps = 60.0f;
}

void FrameTimer::tick(double now, i32 skip_frames, bool game_live, bool paused) {
    // 0x4262f0：若 now < origin 则压低 origin；满 1 秒结算一次 FPS
    if (origin == 0.0 || now < origin) origin = now;
    const double span = now - origin;
    if (span >= 1.0) {
        origin += span;
        fps = float(double(window_frames) / span);
        if (fps <= 65.0f) over65_streak = 0;
        else ++over65_streak;
        if (game_live && !paused) {
            elapsed_total += 60.0;
            elapsed_good += (fps <= 57.0f) ? double(fps) : 60.0;
        }
        window_frames = 0;
    }
    window_frames += skip_frames + 1;
}

float FrameTimer::slow_rate() const {
    if (elapsed_total <= 0) return 0.0f;
    return float(100.0 - (elapsed_good / elapsed_total) * 100.0);
}

}  // namespace th16
