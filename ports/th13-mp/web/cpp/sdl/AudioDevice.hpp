#pragma once
#include "../game/Types.hpp"
#include <functional>
#include <memory>
#include <string>

namespace th13::sdl {
class AudioDevice {
    struct Impl;
    std::unique_ptr<Impl> impl;

public:
    struct RequestObservation {
        enum class Kind { Voice, Stop, Pan, Music } kind;
        i32 id = 0, value = 0;
        bool positional = false;
        float x = 0;
        std::string text;
    };
    std::function<void(const RequestObservation&)> request_observer;
    struct Statistics {
        u64 mixed_frames = 0, nonzero_samples = 0, requests = 0, started = 0;
        u64 music_loops = 0, submitted_frames = 0, dropped_requests = 0;
        u64 stopped = 0;
        u32 music_cursor = 0, queued_frames = 0, sample_rate = 44100;
        u32 active_voices = 0, pending_voices = 0;
        i32 track = -1;
        bool ready = false, device = false, suspended = false, music_paused = false;
    };
    std::string error;
    i32 music_volume = 100;
    bool music_enabled = true;
    bool defer_output = false;
    i32 effect_volume = 80;
    AudioDevice();
    ~AudioDevice();
    bool initialize(bool device = true);
    bool enqueue(i32 id);
    bool enqueue(i32 id, float pan);
    bool stop(i32 id);
    bool set_voice_pan(i32 id, float pan);
    bool observe_voice_pan(i32 id, float pan);
    void apply_voice_pan(i32 id, float pan);
    bool apply_request(const RequestObservation& request);
#ifdef TH13_AUDIO_MIXER_ONLY
    bool initialize_effects_for_test(const std::string& asset_root);
    bool initialize_for_test(const std::string& asset_root, const std::string& music_path = "");
#endif
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
    std::string music_filename() const;
    i32 voice_pan(i32 id) const;
    i32 voice_attenuation(i32 id) const;

private:
    bool queue_voice(i32 id, i32 entry);
    bool validate_voice(i32 id);
    bool validate_stop(i32 id);
    bool validate_music(i32 code, const std::string& text);
    bool apply_stop(i32 id);
    bool apply_music(i32 code, i32 value, const std::string& text);
};
}
