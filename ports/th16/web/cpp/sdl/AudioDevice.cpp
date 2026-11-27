#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_ENCODING
#define MA_NO_THREADING
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-but-set-variable"
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wtautological-compare"
#include "../../../portable/sdl/third_party/stb_vorbis.h"
#define MINIAUDIO_IMPLEMENTATION
#include "../../../portable/sdl/third_party/miniaudio.h"
#pragma clang diagnostic pop

#include "AudioDevice.hpp"

// 共享网页音效输出：预录 se_*.wav 走 WebAudio 独立声部，绕开混音队列的缓冲延迟；
// 非 Emscripten 构建里所有 web_sfx 命令返回 0，自动回落到 miniaudio 声部。
#include <eagler/audio/WebSfx.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

#if defined(__EMSCRIPTEN__) || defined(TH16_HAS_SDL)
#include <SDL3/SDL.h>
#define TH16_AUDIO_SDL 1
#endif

namespace th16::sdl {

namespace web_sfx = eagler::audio::web_sfx;

namespace {

constexpr u32 kMixRate = 44100;
constexpr u32 kMixChannels = 2;

// DirectSound 百分之一 dB → 线性增益
float amplitude(i32 hundredths_db) { return std::pow(10.0f, float(hundredths_db) / 2000.0f); }

// DirectSound 声像（-10000..10000，衰减一侧）→ miniaudio balance（-1..1）
float balance(i32 pan) {
    return pan >= 0 ? 1.0f - std::pow(10.0f, -float(pan) / 2000.0f) : std::pow(10.0f, float(pan) / 2000.0f) - 1.0f;
}

std::string stem_of(const std::string& wave) {
    std::string s = wave;
    const size_t slash = s.find_last_of("/\\");
    if (slash != std::string::npos) s = s.substr(slash + 1);
    if (s.size() >= 4 && s.compare(s.size() - 4, 4, ".wav") == 0) s.resize(s.size() - 4);
    return s;
}

// thbgm.dat 内一段 16 位 PCM 的 miniaudio 数据源：按需 fread，循环点由 ma_data_source 基类处理
struct PcmFileSource {
    ma_data_source_base base{};
    FILE* file = nullptr;
    u64 begin = 0;
    u64 frames = 0;
    u64 cursor = 0;
    u32 channels = 2;
    u32 rate = 44100;
    u32 block = 4;

    static PcmFileSource& self(ma_data_source* ds) { return *reinterpret_cast<PcmFileSource*>(ds); }

    static ma_result on_read(ma_data_source* ds, void* out, ma_uint64 count, ma_uint64* read) {
        PcmFileSource& s = self(ds);
        if (read != nullptr) *read = 0;
        if (s.cursor >= s.frames) return MA_AT_END;
        const u64 n = std::min<u64>(count, s.frames - s.cursor);
        if (std::fseek(s.file, long(s.begin + s.cursor * s.block), SEEK_SET) != 0) return MA_IO_ERROR;
        const size_t got = std::fread(out, s.block, size_t(n), s.file);
        s.cursor += got;
        if (read != nullptr) *read = got;
        if (got == 0) return MA_AT_END;
        return MA_SUCCESS;
    }
    static ma_result on_seek(ma_data_source* ds, ma_uint64 frame) {
        PcmFileSource& s = self(ds);
        if (frame > s.frames) return MA_BAD_SEEK;
        s.cursor = frame;
        return MA_SUCCESS;
    }
    static ma_result on_format(ma_data_source* ds, ma_format* format, ma_uint32* channels, ma_uint32* rate,
                               ma_channel* map, size_t map_cap) {
        PcmFileSource& s = self(ds);
        if (format != nullptr) *format = ma_format_s16;
        if (channels != nullptr) *channels = s.channels;
        if (rate != nullptr) *rate = s.rate;
        if (map != nullptr) ma_channel_map_init_standard(ma_standard_channel_map_default, map, map_cap, s.channels);
        return MA_SUCCESS;
    }
    static ma_result on_cursor(ma_data_source* ds, ma_uint64* cursor) { *cursor = self(ds).cursor; return MA_SUCCESS; }
    static ma_result on_length(ma_data_source* ds, ma_uint64* length) { *length = self(ds).frames; return MA_SUCCESS; }
};

const ma_data_source_vtable kPcmFileVtable{PcmFileSource::on_read, PcmFileSource::on_seek, PcmFileSource::on_format,
                                           PcmFileSource::on_cursor, PcmFileSource::on_length, nullptr, 0};

}  // namespace

struct AudioDevice::Impl {
    struct Voice {
        ma_decoder decoder{};
        ma_sound sound{};
        bool decoded = false;
        bool attached = false;
        ~Voice() {
            web_sfx::erase(&sound);
            if (attached) ma_sound_uninit(&sound);
            if (decoded) ma_decoder_uninit(&decoder);
        }
    };

    // 串流器：PCM 文件段或 OGG 解码器二选一
    struct Stream {
        PcmFileSource pcm{};
        ma_decoder decoder{};
        ma_sound sound{};
        bool pcm_live = false;
        bool decoder_live = false;
        bool attached = false;
        bool looped = false;
        BgmEntry entry;
        ma_data_source* source() { return pcm_live ? reinterpret_cast<ma_data_source*>(&pcm) : reinterpret_cast<ma_data_source*>(&decoder); }
        void close() {
            if (attached) ma_sound_uninit(&sound);
            attached = false;
            if (pcm_live) {
                ma_data_source_uninit(&pcm.base);
                if (pcm.file != nullptr) std::fclose(pcm.file);
                pcm.file = nullptr;
            }
            if (decoder_live) ma_decoder_uninit(&decoder);
            pcm_live = decoder_live = false;
        }
        ~Stream() { close(); }
    };

    struct SfxOutput final : SoundOutput {
        Impl* impl = nullptr;
        bool available(i32 voice) const override { return impl->voice(voice) != nullptr; }
        bool playing(i32 voice) override {
            Voice* v = impl->voice(voice);
            if (v == nullptr) return false;
            return web_sfx::contains(&v->sound) ? web_sfx::playing(&v->sound) : ma_sound_is_playing(&v->sound);
        }
        void stop(i32 voice) override {
            if (Voice* v = impl->voice(voice)) {
                web_sfx::stop(&v->sound);
                ma_sound_stop(&v->sound);
            }
        }
        void set_position(i32 voice, u32 byte) override {
            // 声部缓冲为解码后的 PCM；原版只传 0
            if (Voice* v = impl->voice(voice)) {
                web_sfx::seek(&v->sound, byte / 4, kMixRate);
                ma_sound_seek_to_pcm_frame(&v->sound, byte / 4);
            }
        }
        void set_pan(i32 voice, i32 pan) override {
            if (Voice* v = impl->voice(voice)) {
                web_sfx::pan(&v->sound, balance(pan));
                ma_sound_set_pan_mode(&v->sound, ma_pan_mode_balance);
                ma_sound_set_pan(&v->sound, balance(pan));
            }
        }
        void set_volume(i32 voice, i32 volume) override {
            if (Voice* v = impl->voice(voice)) {
                web_sfx::volume(&v->sound, amplitude(volume));
                ma_sound_set_volume(&v->sound, amplitude(volume));
            }
        }
        void play(i32 voice, u32 flags) override {
            if (Voice* v = impl->voice(voice)) {
                ma_sound_set_looping(&v->sound, (flags & 1u) ? MA_TRUE : MA_FALSE);
                if (!web_sfx::play(&v->sound, (flags & 1u) != 0)) ma_sound_start(&v->sound);
            }
        }
    };

    struct StreamOutput final : MusicOutput {
        Impl* impl = nullptr;
        AudioDevice* owner = nullptr;
        // 0x471270：Stop + SetCurrentPosition(0)；close_file 时另关 thbgm.dat 读取器
        void stop(bool close_file) override {
            Stream& s = impl->stream;
            if (s.attached) {
                ma_sound_stop(&s.sound);
                ma_sound_seek_to_pcm_frame(&s.sound, 0);
            }
            if (close_file) s.close();
        }
        // 0x470bb0：重建 DirectSound 缓冲（音量回到配置值）并绑定曲目
        void open(const BgmEntry& entry) override { owner->open_stream(entry); }
        void seek(const BgmEntry&, u32 offset) override {
            Stream& s = impl->stream;
            if (s.attached) ma_sound_seek_to_pcm_frame(&s.sound, offset / std::max<u32>(1, s.entry.block_align));
        }
        void play(bool looped, u32 offset) override {
            Stream& s = impl->stream;
            if (!s.attached) return;
            s.looped = looped;
            ma_sound_set_looping(&s.sound, looped ? MA_TRUE : MA_FALSE);
            ma_sound_seek_to_pcm_frame(&s.sound, offset / std::max<u32>(1, s.entry.block_align));
        }
        void start() override {
            Stream& s = impl->stream;
            if (s.attached) ma_sound_start(&s.sound);
        }
        void pause() override {
            Stream& s = impl->stream;
            if (s.attached) ma_sound_stop(&s.sound);
        }
        void resume() override {
            Stream& s = impl->stream;
            if (s.attached) ma_sound_start(&s.sound);
        }
        void halt() override {
            Stream& s = impl->stream;
            if (s.attached) ma_sound_stop(&s.sound);
        }
        void set_volume(i32 base) override { owner->apply_music_volume(base); }
        // 0x471b00：换曲保持播放时刻
        void switch_loop(const BgmEntry& entry) override {
            const double at = position_seconds();
            owner->open_stream(entry);
            restart_at(at);
        }
        double position_seconds() override {
            Stream& s = impl->stream;
            if (!s.attached) return 0.0;
            ma_uint64 cursor = 0;
            ma_sound_get_cursor_in_pcm_frames(&s.sound, &cursor);
            return double(cursor) / double(std::max<u32>(1, s.entry.sample_rate));
        }
        // 0x471c90：Stop → 字节偏移 = int(采样率 × 秒) × 块长 − 块长（≥ 0）→ 重开并从该偏移播放
        void restart_at(double seconds) override {
            Stream& s = impl->stream;
            if (!s.attached) return;
            const u32 block = std::max<u32>(1, s.entry.block_align);
            i64 frame = i64(double(s.entry.sample_rate) * seconds) - 1;
            if (frame < 0) frame = 0;
            ma_sound_stop(&s.sound);
            ma_sound_set_looping(&s.sound, s.looped ? MA_TRUE : MA_FALSE);
            ma_sound_seek_to_pcm_frame(&s.sound, ma_uint64(frame));
            ma_sound_start(&s.sound);
            (void)block;
        }
    };

    ma_engine engine{};
    bool engine_ready = false;
    bool ready = false;
    bool suspended = false;
    bool refill = true;
    bool music_on = true;
    i32 music_base = 0;          // 最近一次 0x4711f0 的 base（关/开音乐时按它重算增益）
    u32 min_queued = 0xffffffffu;
    u64 mixed_frames = 0;
    u32 mix_failures = 0;
    std::array<std::unique_ptr<Voice>, kSoundVoiceCount> voices;
    std::array<std::vector<u8>, kSoundSampleCount> samples;
    Stream stream;
    SfxOutput sfx;
    StreamOutput music;
#ifdef TH16_AUDIO_SDL
    SDL_AudioStream* sdl_stream = nullptr;
#endif

    ~Impl() {
        stream.close();
        for (auto& v : voices) v.reset();
#ifdef TH16_AUDIO_SDL
        if (sdl_stream != nullptr) SDL_DestroyAudioStream(sdl_stream);
#endif
        if (engine_ready) ma_engine_uninit(&engine);
    }

    Voice* voice(i32 id) { return id >= 0 && id < i32(voices.size()) ? voices[size_t(id)].get() : nullptr; }
};

AudioDevice::AudioDevice() : impl(std::make_unique<Impl>()) {
    impl->sfx.impl = impl.get();
    impl->music.impl = impl.get();
    impl->music.owner = this;
}

AudioDevice::~AudioDevice() = default;

SoundOutput& AudioDevice::sfx_output() { return impl->sfx; }
MusicOutput& AudioDevice::music_output() { return impl->music; }
bool AudioDevice::ready() const { return impl->ready; }
bool AudioDevice::music_open() const { return impl->stream.attached; }
bool AudioDevice::music_playing() const { return impl->stream.attached && ma_sound_is_playing(&impl->stream.sound); }
u64 AudioDevice::music_cursor() const {
    if (!impl->stream.attached) return 0;
    ma_uint64 cursor = 0;
    ma_sound_get_cursor_in_pcm_frames(&impl->stream.sound, &cursor);
    return cursor;
}

bool AudioDevice::initialize(Archive& archive, bool device) {
    Impl& a = *impl;
    if (a.ready) return true;
    ma_engine_config config = ma_engine_config_init();
    config.noDevice = MA_TRUE;
    config.channels = kMixChannels;
    config.sampleRate = kMixRate;
    config.defaultVolumeSmoothTimeInPCMFrames = 0;
    if (ma_engine_init(&config, &a.engine) != MA_SUCCESS) { error = "audio: cannot initialize mixer"; return false; }
    a.engine_ready = true;

#ifdef TH16_AUDIO_SDL
    if (device) {
        SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES, "1024");
        const SDL_AudioSpec spec{SDL_AUDIO_F32, int(kMixChannels), int(kMixRate)};
        if (SDL_InitSubSystem(SDL_INIT_AUDIO)) a.sdl_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
        if (a.sdl_stream == nullptr) { error = std::string("audio: ") + SDL_GetError(); return false; }
    }
#else
    (void)device;
#endif

    for (size_t n = 0; n < a.samples.size(); ++n) {
        if (!archive.read(kSoundSamples[n], a.samples[n])) { error = std::string("audio: missing ") + kSoundSamples[n]; return false; }
    }
    // 0x45d510：声部按定义行 DAT_004a2a30 +0 匹配；同一采样可被多个声部共用（各自独立解码源）
    for (i32 v = 0; v < kSoundVoiceCount; ++v) {
        const SoundDefinition* def = nullptr;
        for (const SoundDefinition& row : kSoundDefinitions) {
            if (row.voice == v) { def = &row; break; }
        }
        if (def == nullptr || def->sample < 0 || def->sample >= kSoundSampleCount) continue;
        auto voice = std::make_unique<Impl::Voice>();
        const std::vector<u8>& bytes = a.samples[size_t(def->sample)];
        ma_decoder_config decode = ma_decoder_config_init(ma_format_f32, kMixChannels, kMixRate);
        if (ma_decoder_init_memory(bytes.data(), bytes.size(), &decode, &voice->decoder) != MA_SUCCESS) {
            error = std::string("audio: invalid wav ") + kSoundSamples[size_t(def->sample)];
            return false;
        }
        voice->decoded = true;
        if (ma_sound_init_from_data_source(&a.engine, &voice->decoder, MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, &voice->sound) != MA_SUCCESS) {
            error = "audio: cannot attach voice";
            return false;
        }
        voice->attached = true;
        if (device) {
            // 整段解码后交给共享网页输出；上传失败（非浏览器/无 AudioContext）则留在 miniaudio 声部
            ma_uint64 frames = 0, read = 0;
            if (ma_decoder_get_length_in_pcm_frames(&voice->decoder, &frames) == MA_SUCCESS && frames != 0 && frames <= UINT32_MAX / 8) {
                std::vector<float> pcm(size_t(frames) * kMixChannels);
                if (ma_decoder_read_pcm_frames(&voice->decoder, pcm.data(), frames, &read) == MA_SUCCESS && read == frames) {
                    web_sfx::upload(&voice->sound, pcm.data(), unsigned(frames), kMixChannels, kMixRate);
                }
                ma_decoder_seek_to_pcm_frame(&voice->decoder, 0);
            }
        }
        a.voices[size_t(v)] = std::move(voice);
    }
#ifdef TH16_AUDIO_SDL
    if (a.sdl_stream != nullptr) SDL_ResumeAudioStreamDevice(a.sdl_stream);
#endif
    a.ready = true;
    error.clear();
    return true;
}

// 0x470bb0：释放旧缓冲、按条目重建；数据源：thbgm.dat 段（首选）或 OGG
void AudioDevice::open_stream(const BgmEntry& entry) {
    Impl& a = *impl;
    Impl::Stream& s = a.stream;
    s.close();
    s.entry = entry;
    const u32 block = std::max<u32>(1, entry.block_align);
    const u64 frames = u64(entry.size) / block;
    bool opened = false;
    if (!thbgm_path.empty()) {
        if (FILE* f = std::fopen(thbgm_path.c_str(), "rb")) {
            ma_data_source_config cfg = ma_data_source_config_init();
            cfg.vtable = &kPcmFileVtable;
            if (ma_data_source_init(&cfg, &s.pcm.base) == MA_SUCCESS) {
                s.pcm.file = f;
                s.pcm.begin = entry.offset;
                s.pcm.frames = frames;
                s.pcm.cursor = 0;
                s.pcm.channels = entry.channels;
                s.pcm.rate = entry.sample_rate;
                s.pcm.block = block;
                s.pcm_live = true;
                opened = true;
            } else {
                std::fclose(f);
            }
        }
    }
    if (!opened) {
        const std::string path = music_directory + "/" + stem_of(entry.name) + ".ogg";
        ma_decoder_config cfg = ma_decoder_config_init(ma_format_s16, entry.channels, entry.sample_rate);
        if (ma_decoder_init_file(path.c_str(), &cfg, &s.decoder) != MA_SUCCESS) {
            error = "audio: cannot open " + path;
            return;
        }
        s.decoder_live = true;
        ma_uint64 length = 0;
        if (ma_decoder_get_length_in_pcm_frames(&s.decoder, &length) != MA_SUCCESS || length != frames) {
            error = "audio: PCM frame count differs from thbgm.fmt: " + path;
            s.close();
            return;
        }
        opened = true;
    }
    // 循环区间 [+0x18 循环起点, +0x1c 末尾)；length = 0 的曲目不循环（MusicManager 按 play(looped) 传入）
    ma_data_source_set_loop_point_in_pcm_frames(s.source(), u64(entry.loop_start) / block, frames);
    // 原版 BGM 使用固定采样率，按指定的 PCM 帧开始播放。关闭无用的变调，避免重采样延后一帧。
    constexpr ma_uint32 kMusicFlags = MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_NO_PITCH;
    if (ma_sound_init_from_data_source(&a.engine, s.source(), kMusicFlags, nullptr, &s.sound) != MA_SUCCESS) {
        error = "audio: cannot attach music stream";
        s.close();
        return;
    }
    s.attached = true;
    apply_music_volume(0);
}

// 0x4711f0：(1 − (1 − m/100)²) × (base + 5000) − 5000，m = 0 → -10000；未接 MusicManager 时按 m = 100
void AudioDevice::apply_music_volume(i32 base) {
    Impl::Stream& s = impl->stream;
    impl->music_base = base;
    if (!s.attached) return;
    const i32 ds = music != nullptr ? music->ds_volume(base) : base;
    ma_sound_set_volume(&s.sound, impl->music_on ? amplitude(ds) : 0.0f);
}

void AudioDevice::set_music_enabled(bool on) {
    impl->music_on = on;
    apply_music_volume(impl->music_base);
}

bool AudioDevice::music_enabled() const { return impl->music_on; }

u32 AudioDevice::queued_frames() const {
#ifdef TH16_AUDIO_SDL
    if (impl->sdl_stream != nullptr) return u32(std::max(0, SDL_GetAudioStreamQueued(impl->sdl_stream)) / i32(sizeof(float) * kMixChannels));
#endif
    return 0;
}

u32 AudioDevice::take_min_queued_frames() {
    const u32 value = impl->min_queued == 0xffffffffu ? queued_frames() : impl->min_queued;
    impl->min_queued = 0xffffffffu;
    return value;
}

std::string AudioDevice::status() const {
    const Impl& a = *impl;
    i32 queued = -1;
#ifdef TH16_AUDIO_SDL
    if (a.sdl_stream != nullptr) queued = std::max(0, SDL_GetAudioStreamQueued(a.sdl_stream)) / i32(sizeof(float) * kMixChannels);
#endif
    char line[256];
    std::snprintf(line, sizeof(line), "ready=%d stream=%d queued=%d mixed=%llu mix_fail=%u music=%d/%d/%llu cur=%s q=%zu err=%s",
                  a.ready ? 1 : 0, queued >= 0 ? 1 : 0, queued, (unsigned long long)a.mixed_frames, a.mix_failures,
                  music_open() ? 1 : 0, music_playing() ? 1 : 0, (unsigned long long)music_cursor(),
                  music != nullptr && !music->current.empty() ? music->current.c_str() : "-", music != nullptr ? music->queue.size() : size_t(0),
                  error.c_str());
    return line;
}

bool AudioDevice::mix(float* stereo, u32 frames) {
    Impl& a = *impl;
    if (!a.ready || stereo == nullptr) return false;
    ma_uint64 read = 0;
    if (ma_engine_read_pcm_frames(&a.engine, stereo, frames, &read) != MA_SUCCESS || read != frames) { ++a.mix_failures; return false; }
    for (u32 n = 0; n < frames * kMixChannels; ++n) stereo[n] = std::clamp(stereo[n], -1.0f, 1.0f);
    a.mixed_frames += frames;
    return true;
}

void AudioDevice::pump() {
#ifdef TH16_AUDIO_SDL
    Impl& a = *impl;
    if (!a.ready || a.sdl_stream == nullptr || a.suspended) return;
    // 队列保持在 2048..3072 帧之间（约 46..70 ms），每帧最多补三块
    constexpr i32 kLow = 2048, kHigh = 3072, kChunk = 1024;
    i32 queued = std::max(0, SDL_GetAudioStreamQueued(a.sdl_stream)) / i32(sizeof(float) * kMixChannels);
    a.min_queued = std::min(a.min_queued, u32(queued));
    if (!a.refill && queued < kLow) a.refill = true;
    if (!a.refill) return;
    if (queued >= kHigh) { a.refill = false; return; }
    float pcm[kChunk * kMixChannels];
    for (int n = 0; n < 3 && queued < kHigh; ++n) {
        const i32 frames = std::min(kChunk, kHigh - queued);
        if (!mix(pcm, u32(frames))) return;
        if (!SDL_PutAudioStreamData(a.sdl_stream, pcm, int(frames * sizeof(float) * kMixChannels))) return;
        queued += frames;
    }
    if (queued >= kHigh) a.refill = false;
#endif
}

void AudioDevice::suspend(bool value) {
    Impl& a = *impl;
    if (a.suspended == value) return;
    a.suspended = value;
    web_sfx::suspend(value);
#ifdef TH16_AUDIO_SDL
    if (a.sdl_stream != nullptr) {
        if (value) SDL_PauseAudioStreamDevice(a.sdl_stream);
        else SDL_ResumeAudioStreamDevice(a.sdl_stream);
    }
#endif
}

}  // namespace th16::sdl
