// The existing repository's stb_vorbis decoder is used only for the installed
// OGG soundtrack. All voice IDs/gains and loop boundaries are TH13's own data.
#include "../../../portable/sdl/third_party/stb_vorbis.h"
#include "AudioDevice.hpp"
#include "../game/SoundEffects.hpp"
#include "../game/MusicLayout.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <deque>
#include <vector>
#ifndef TH13_AUDIO_MIXER_ONLY
#include <SDL3/SDL.h>
#endif
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
EM_JS(void, th13_publish_audio, (double mixed, double nonzero, double requests,
      double started, double submitted, double loops, int queued, int track,
      int cursor, int paused, int suspended, int device, int failed), {
    Module.th13Audio = {mixedFrames:mixed, nonzeroSamples:nonzero, requests:requests,
      started:started, submittedFrames:submitted, musicLoops:loops,
      queuedFrames:queued, track:track, musicCursor:cursor,
      paused:!!paused, suspended:!!suspended, device:!!device,
      failed:!!failed, sampleRate:44100};
});
#endif
namespace th13::sdl {
namespace {
u16 half(const u8* p) { u16 v; std::memcpy(&v, p, 2); return v; }
u32 word(const u8* p) { u32 v; std::memcpy(&v, p, 4); return v; }
bool read_file(const std::string& path, std::vector<u8>& out) {
    auto* f = std::fopen(path.c_str(), "rb");
    if (!f) return false;
    if (std::fseek(f, 0, SEEK_END)) { std::fclose(f); return false; }
    const long size = std::ftell(f);
    if (size < 0 || std::fseek(f, 0, SEEK_SET)) { std::fclose(f); return false; }
    out.resize(size_t(size));
    const bool ok = std::fread(out.data(), 1, out.size(), f) == out.size();
    std::fclose(f); return ok;
}
float amplitude(i32 hundredths_db) { return std::pow(10.f, float(hundredths_db) / 2000.f); }
}
struct AudioDevice::Impl {
    struct Sample { std::vector<float> pcm; u32 frames = 0; };
    struct Voice { u32 cursor = 0; bool playing = false; };
    struct Request { i32 code, value, step = 0; std::string text; };
    std::array<Sample, 60> samples;
    std::array<Voice, 71> voices;
    std::array<const SoundDefinition*, 71> definitions{};
    std::array<bool, 71> pending{};
    std::deque<Request> commands;
    std::array<std::string, 16> prepared;
    std::string base_music; //004e13c0 changes on play2, not variant9.
    MusicLayout layout;
    stb_vorbis* music = nullptr;
#ifndef TH13_AUDIO_MIXER_ONLY
    SDL_AudioStream* stream = nullptr;
#endif
    Statistics stats;
    u32 decoded_frames = 0, music_cursor = 0;
    i32 track = -1, fade = 0, fade_total = 0;
    bool music_playing = false, music_paused = false, suspended = false;
    float music_fraction = 0;
    std::array<float, 4096> music_buffer{};
    u32 music_buffer_size = 0, music_buffer_at = 0;
    std::array<float, 2> held_music{};
    ~Impl() {
#ifndef TH13_AUDIO_MIXER_ONLY
        if (stream) SDL_DestroyAudioStream(stream);
#endif
        if (music) stb_vorbis_close(music);
    }
    bool decode_wave(const std::vector<u8>& bytes, Sample& result) {
        if (bytes.size() < 12 || std::memcmp(bytes.data(), "RIFF", 4) ||
            std::memcmp(bytes.data() + 8, "WAVE", 4)) return false;
        u16 format = 0, channels = 0, bits = 0, align = 0;
        u32 rate = 0, size = 0; const u8* data = nullptr;
        for (size_t at = 12; at + 8 <= bytes.size();) {
            const auto* p = bytes.data() + at;
            const u32 length = word(p + 4);
            if (u64(at) + 8 + length > bytes.size()) return false;
            if (!std::memcmp(p, "fmt ", 4) && length >= 16) {
                format = half(p + 8); channels = half(p + 10); rate = word(p + 12);
                align = half(p + 20); bits = half(p + 22);
            }
            if (!std::memcmp(p, "data", 4)) { data = p + 8; size = length; }
            at += 8 + size_t(length) + (length & 1);
        }
        if (!data || format != 1 || (channels != 1 && channels != 2) ||
            !rate || (bits != 8 && bits != 16) || align != channels * (bits / 8) ||
            size % align) return false;
        const u32 source_frames = size / align;
        if (!source_frames) return false;
        result.frames = u32((u64(source_frames) * 44100 + rate - 1) / rate);
        result.pcm.resize(size_t(result.frames) * 2);
        auto sample = [&](u32 frame, u32 channel) {
            const auto* p = data + size_t(frame) * align + (channels == 1 ? 0 : channel) * (bits / 8);
            if (bits == 8) return (float(*p) - 128.f) / 128.f;
            i16 v; std::memcpy(&v, p, 2); return float(v) / 32768.f;
        };
        for (u32 n = 0; n < result.frames; ++n) {
            const double position = double(n) * rate / 44100.;
            const u32 a = std::min(u32(position), source_frames - 1);
            const u32 b = std::min(a + 1, source_frames - 1);
            const float frac = float(position - a);
            for (u32 c = 0; c < 2; ++c)
                result.pcm[size_t(n) * 2 + c] = sample(a, c) * (1.f - frac) + sample(b, c) * frac;
        }
        return true;
    }
    bool seek(u32 frame) {
        const auto* t = layout.track(track);
        if (!music || !t || frame >= t->frames()) return false;
        // The installed thbgm.dat is one OGG stream over the original virtual
        // PCM archive (including its 16-byte header). Offsets remain byte units.
        if (!stb_vorbis_seek(music, t->offset / 4 + frame)) return false;
        music_cursor = frame; music_buffer_size = music_buffer_at = 0;
        music_fraction = 0; held_music = {}; return true;
    }
    bool music_sample(float& l, float& r) {
        auto* t = layout.track(track);
        if (!t || !music) return false;
        if (music_cursor >= t->frames()) {
            if (!seek(t->loop_frame())) return false;
            ++stats.music_loops;
        }
        if (music_buffer_at == music_buffer_size) {
            const u32 want = std::min(2048u, t->frames() - music_cursor);
            const int read = stb_vorbis_get_samples_float_interleaved(music, 2,
                music_buffer.data(), int(want * 2));
            if (read != int(want)) return false;
            music_buffer_at = 0; music_buffer_size = u32(read);
        }
        l = music_buffer[size_t(music_buffer_at) * 2];
        r = music_buffer[size_t(music_buffer_at) * 2 + 1];
        ++music_buffer_at; ++music_cursor; return true;
    }
};
AudioDevice::AudioDevice() : impl(std::make_unique<Impl>()) {}
AudioDevice::~AudioDevice() = default;
void AudioDevice::close() { impl = std::make_unique<Impl>(); error.clear(); }
bool AudioDevice::initialize(bool device) {
    if (impl->stats.ready) return true;
    // A failed attempt is never retained as a partially working silent device.
    close(); auto& a = *impl;
    for (const auto& row : sound_definitions) a.definitions[u32(row.id)] = &row;
    std::vector<u8> bytes;
    for (u32 n = 0; n < sound_samples.size(); ++n) {
        const std::string path = std::string("/assets/") + sound_samples[n];
        if (!read_file(path, bytes) || !a.decode_wave(bytes, a.samples[n])) {
            error = "Missing or invalid original TH13 sound: " + path; return false;
        }
    }
    if (!read_file("/assets/thbgm.fmt", bytes) || !a.layout.open(bytes.data(), u32(bytes.size()))) {
        error = a.layout.error.empty() ? "Missing /assets/thbgm.fmt" : a.layout.error; return false;
    }
    int decode_error = 0;
    a.music = stb_vorbis_open_filename("/assets/thbgm.ogg", &decode_error, nullptr);
    if (!a.music) { error = "Unable to open original TH13 OGG archive: " + std::to_string(decode_error); return false; }
    const auto info = stb_vorbis_get_info(a.music);
    a.decoded_frames = stb_vorbis_stream_length_in_samples(a.music);
    u64 expected = 0;
    for (u32 n = 0; n < a.layout.count(); ++n) {
        auto* t = a.layout.track(i32(n));
        expected = std::max(expected, (u64(t->offset) + t->length) / 4);
    }
    if (info.channels != 2 || info.sample_rate != 44100 || a.decoded_frames != expected) {
        error = "TH13 OGG archive does not match original PCM layout"; return false;
    }
#ifndef TH13_AUDIO_MIXER_ONLY
    if (device) {
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) { error = SDL_GetError(); return false; }
        SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES, "1024");
        const SDL_AudioSpec spec{SDL_AUDIO_F32, 2, 44100};
        a.stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
        if (!a.stream || !SDL_ResumeAudioStreamDevice(a.stream)) {
            error = SDL_GetError(); return false;
        }
    }
#else
    if (device) { error = "Mixer-only diagnostic binary has no SDL output device"; return false; }
#endif
    a.stats.ready = true; a.stats.device = device; return true;
}
bool AudioDevice::enqueue(i32 id) {
    auto& a = *impl;
    if (!a.stats.ready || id < 0 || id >= i32(a.pending.size()) || !a.definitions[u32(id)]) {
        error = "Invalid or uninitialized TH13 sound request: " + std::to_string(id); return false;
    }
    ++a.stats.requests;
    a.pending[u32(id)] = true; // Original frame queue coalesces duplicate voice IDs.
    return true;
}
bool AudioDevice::trance_music(bool active) {
    const auto& base = impl->base_music;
    const auto dot = base.find('.');
    if (dot == std::string::npos) { error = "Trance music has no current base track"; return false; }
    return queue_music(9, 0, base.substr(0, dot) + (active ? "b.wav" : ".wav"));
}
bool AudioDevice::queue_music(i32 code, i32 value, const std::string& text) {
    auto& a = *impl;
    if (!a.stats.ready || code < 1 || code > 9 || text.size() >= 256 || a.commands.size() >= 31) {
        error = "Invalid or full TH13 music command queue"; return false;
    }
    a.commands.push_back({code, value, 0, text}); return true;
}
void AudioDevice::update() {
    auto& a = *impl;
    if (!a.stats.ready || !error.empty()) return;
    for (u32 n = 0; n < a.pending.size(); ++n) if (a.pending[n]) {
        a.pending[n] = false; a.voices[n] = {0, true}; ++a.stats.started;
    }
    if (a.fade > 0 && !a.music_paused && --a.fade == 0) a.music_playing = false;
    if (a.commands.empty()) return;
    auto& c = a.commands.front(); bool remove = true;
    switch (c.code) {
    case 1:
        if (c.value < 0 || c.value >= i32(a.prepared.size()) || a.layout.index(c.text) < 0) {
            error = "Invalid TH13 music preload slot or filename"; return;
        }
        a.prepared[u32(c.value)] = c.text; break;
    case 2:
        // 00460dd0: stop/load/select/fill/start at steps 0/1/2/3/4,
        // retire at step 7. There is no fake successful missing-song fallback.
        remove = false;
        if (c.step == 0) a.music_playing = false;
        else if (c.step == 1) {
            std::string name = c.text;
            if (c.value >= 0) {
                if (c.value >= i32(a.prepared.size())) { error = "Invalid TH13 music play slot"; return; }
                name = a.prepared[u32(c.value)];
            }
            a.track = a.layout.index(name);
            if (a.track < 0) { error = "Missing TH13 music track: " + name; return; }
            a.base_music = a.layout.track(a.track)->filename;
        } else if (c.step == 3) {
            if (!a.seek(0)) { error = "TH13 music initial seek failed"; return; }
        } else if (c.step == 4) {
            a.music_playing = true; a.music_paused = false; a.fade = a.fade_total = 0;
        } else if (c.step >= 7) remove = true;
        ++c.step; break;
    case 3: case 4: a.music_playing = false; a.track = -1; break;
    case 5:
        a.fade = a.fade_total = std::max(0, c.value * 60);
        if (!a.fade) a.music_playing = false;
        break;
    case 6: pause_music(true); break;
    case 7: pause_music(false); break;
    case 8: break; // Gain reads music_volume at every mixed block.
    case 9: {
        auto* old = a.layout.track(a.track);
        const double seconds = old ? double(a.music_cursor) / old->rate : 0.;
        a.track = a.layout.index(c.text);
        auto* next = a.layout.track(a.track);
        if (!next) { error = "Missing TH13 alternate music track: " + c.text; return; }
        u32 frame = u32(seconds * next->rate);
        if (frame >= next->frames()) frame = next->loop_frame() +
            (frame - next->loop_frame()) % (next->frames() - next->loop_frame());
        if (!a.seek(frame)) { error = "TH13 alternate music seek failed"; return; }
        break;
    }
    }
    if (remove) a.commands.pop_front();
}
bool AudioDevice::finish_music_requests() {
    for (u32 n = 0; n < 256 && !impl->commands.empty() && error.empty(); ++n) update();
    if (!impl->commands.empty() && error.empty()) error = "TH13 music commands did not drain";
    return error.empty();
}
void AudioDevice::pause_music(bool value) { impl->music_paused = value; }
void AudioDevice::suspend(bool value) {
    auto& a = *impl; a.suspended = value;
#ifndef TH13_AUDIO_MIXER_ONLY
    if (a.stream) {
        const bool ok = value ? SDL_PauseAudioStreamDevice(a.stream) : SDL_ResumeAudioStreamDevice(a.stream);
        if (!ok) error = SDL_GetError();
    }
#endif
}
bool AudioDevice::seek_music_frames(u32 frame) {
    if (!impl->seek(frame)) { error = "TH13 music seek outside track or decoder failure"; return false; }
    return true;
}
bool AudioDevice::mix(float* pcm, u32 frames) {
    auto& a = *impl;
    if (!a.stats.ready || !pcm || !error.empty()) return false;
    std::fill(pcm, pcm + size_t(frames) * 2, 0.f);
    if (a.suspended) return true;
    for (u32 id = 0; id < a.voices.size(); ++id) {
        auto& v = a.voices[id]; if (!v.playing) continue;
        const auto& d = *a.definitions[id]; const auto& sample = a.samples[u32(d.sample)];
        // SE 主音量（原版 0x4613f3，每 voice）：0 静音；否则 attenuation =
        // round((1-(se/100-1)²)·(def+5000)) - 5000（hundredths dB，amplitude
        // 同曲线换算）。se=100 时退化为原 per-definition 值。
        const float se = float(std::clamp(effect_volume, 0, 100)) / 100.f;
        const float off = se - 1.f;
        const i32 attenuation =
            effect_volume <= 0 ? -10000
                               : i32(std::lround((1.f - off * off) * float(d.volume + 5000))) - 5000;
        const float gain = amplitude(attenuation);
        for (u32 frame = 0; frame < frames; ++frame) {
            if (v.cursor == sample.frames) {
                if (d.flags & 1) v.cursor = 0;
                else { v.playing = false; break; }
            }
            for (u32 c = 0; c < 2; ++c)
                pcm[size_t(frame) * 2 + c] += sample.pcm[size_t(v.cursor) * 2 + c] * gain;
            ++v.cursor;
        }
    }
    const auto* t = a.layout.track(a.track);
    if (a.music_playing && !a.music_paused && t) {
        const float volume = float(std::clamp(music_volume, 0, 100)) / 100.f;
        const float inverse = 1.f - volume;
        const i32 base = a.fade_total ? i32(i64(a.fade) * 5000 / a.fade_total) - 5000 : 0;
        const float gain = !music_enabled || volume == 0.f ? 0.f : amplitude(i32((1.f - inverse * inverse) * (base + 5000)) - 5000);
        for (u32 frame = 0; frame < frames; ++frame) {
            // Normal tracks are 44100 Hz. Original alternate/spirit tracks
            // carry 22050 Hz PCM in the same archive and consume half as fast.
            if (a.music_fraction <= 0.f) {
                if (!a.music_sample(a.held_music[0], a.held_music[1])) {
                    error = "Original TH13 music decoder ended before loop boundary"; return false;
                }
                a.music_fraction += 1.f;
            }
            pcm[size_t(frame) * 2] += a.held_music[0] * gain;
            pcm[size_t(frame) * 2 + 1] += a.held_music[1] * gain;
            a.music_fraction -= float(t->rate) / 44100.f;
        }
    }
    a.stats.mixed_frames += frames;
    for (size_t n = 0; n < size_t(frames) * 2; ++n) {
        pcm[n] = std::clamp(pcm[n], -1.f, 1.f);
        if (pcm[n] != 0.f) ++a.stats.nonzero_samples;
    }
    return true;
}
AudioDevice::Statistics AudioDevice::statistics() const {
    const auto& a = *impl; auto s = a.stats;
    s.music_cursor = a.music_cursor; s.track = a.track;
    s.music_paused = a.music_paused; s.suspended = a.suspended;
#ifndef TH13_AUDIO_MIXER_ONLY
    if (a.stream) s.queued_frames = u32(std::max(0, SDL_GetAudioStreamQueued(a.stream))) / 8;
#endif
    return s;
}
void AudioDevice::pump() {
    auto& a = *impl;
#ifndef TH13_AUDIO_MIXER_ONLY
    if (a.stats.ready && a.stream && !a.suspended && error.empty()) {
        const int queued_bytes = SDL_GetAudioStreamQueued(a.stream);
        if (queued_bytes < 0) { error = SDL_GetError(); return; }
        // Top up according to actual SDL consumption, never a 735-frame/game
        // tick cap. 8192 frames/pump exceeds 44100 Hz even at 10 pump calls/s.
        u32 queued = u32(queued_bytes) / 8;
        constexpr u32 target = 3072, chunk = 1024, budget = 8192;
        for (u32 spent = 0; queued < target && spent < budget;) {
            float pcm[chunk * 2];
            const u32 frames = std::min(chunk, target - queued);
            if (!mix(pcm, frames)) return;
            if (!SDL_PutAudioStreamData(a.stream, pcm, int(frames * 8))) { error = SDL_GetError(); return; }
            a.stats.submitted_frames += frames; queued += frames; spent += frames;
        }
    }
#endif
#ifdef __EMSCRIPTEN__
    const auto s = statistics();
    th13_publish_audio(double(s.mixed_frames), double(s.nonzero_samples), double(s.requests),
        double(s.started), double(s.submitted_frames), double(s.music_loops),
        int(s.queued_frames), s.track, int(s.music_cursor), s.music_paused,
        s.suspended, s.device, !error.empty());
#endif
}
}
