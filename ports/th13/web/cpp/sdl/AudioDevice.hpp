#pragma once
#include "../game/Types.hpp"
#include <memory>
#include <string>

namespace th13::sdl {
// SDL output and no-device diagnostic mixing share the exact same PCM path.
// Diagnostic mode has no device, but still requires all original assets.
class AudioDevice {
    struct Impl;
    std::unique_ptr<Impl> impl;
public:
    struct Statistics {
        u64 mixed_frames = 0, nonzero_samples = 0, requests = 0, started = 0;
        u64 music_loops = 0, submitted_frames = 0;
        u32 music_cursor = 0, queued_frames = 0, sample_rate = 44100;
        i32 track = -1;
        bool ready = false, device = false, suspended = false, music_paused = false;
    };
    std::string error;
    i32 music_volume = 100;
    bool music_enabled = true;
    // SE 主音量（原版 DAT_004dc893→DAT_004e4770，0x4613f3 每 voice 曲线：
    // 0 = 静音；否则 gain=atten(round((1-(v/100-1)²)·(def+5000))-5000)）。
    // 默认 80 = 原版存档缺省 0x5064 高字节。
    i32 effect_volume = 80;
    AudioDevice();
    ~AudioDevice();
    bool initialize(bool device = true);
    bool enqueue(i32 id);
    bool queue_music(i32 code, i32 value, const std::string& text = "");
    bool trance_music(bool active);
    bool finish_music_requests();
    void pause_music(bool);
    void suspend(bool);
    void update();
    void pump();
    void close();
    bool mix(float* stereo, u32 frames);
    bool seek_music_frames(u32 frame);
    Statistics statistics() const;
};
}
