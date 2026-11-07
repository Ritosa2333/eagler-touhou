#pragma once
#include <algorithm>
#include <unordered_map>
#include <vector>

// Output-service double only. AudioDevice's decoder, frame-channel table,
// attenuation and music/PCM separation are the production implementation.
namespace eagler::audio::web_sfx {
struct Voice {
    std::vector<float> pcm;
    unsigned frames = 0, cursor = 0, starts = 0;
    float gain = 1.f, balance = 0.f;
    bool active = false, loop = false;
};
inline std::unordered_map<const void*, Voice> voices;
inline bool paused = false, reject_upload = false, reject_play = false;
inline bool upload(const void* key, const void* pcm, unsigned frames,
                   unsigned channels, unsigned rate, int format = 0) {
    if (reject_upload || !pcm || !frames || channels != 2 || rate != 44100 || format) return false;
    auto& v = voices[key];
    v = Voice{};
    v.frames = frames;
    v.pcm.assign(static_cast<const float*>(pcm), static_cast<const float*>(pcm) + frames * channels);
    return true;
}
inline bool contains(const void* key) { return voices.count(key) != 0; }
inline void erase(const void* key) { voices.erase(key); }
inline bool play(const void* key, bool loop = false) {
    if (reject_play || !contains(key)) return false;
    auto& v = voices.at(key);
    v.active = true; v.loop = loop; ++v.starts;
    return true;
}
inline void stop(const void* key) { if (contains(key)) voices.at(key).active = false; }
inline void seek(const void* key, unsigned frame, unsigned) { if (contains(key)) voices.at(key).cursor = frame; }
inline void volume(const void* key, float gain) { if (contains(key)) voices.at(key).gain = gain; }
inline void pan(const void* key, float balance) { if (contains(key)) voices.at(key).balance = balance; }
inline unsigned cursor(const void* key) { return contains(key) ? voices.at(key).cursor : 0; }
inline bool playing(const void* key) { return contains(key) && voices.at(key).active; }
inline void suspend(bool value) { paused = value; }
}
