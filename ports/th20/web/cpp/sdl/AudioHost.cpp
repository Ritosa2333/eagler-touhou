// Software mixer for the TH_SDL3 build, following the TH10 web runtime's
// miniaudio (no device IO) -> SDL3 stream split. Original PCM, DirectSound
// gain units and notification offsets remain part of TH20's recovered audio
// logic; no PCM crosses a JS message port.
#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_WAV
#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_ENCODING
#define MA_NO_THREADING
// File-backed Vorbis is the canonical BGM source. Keeping stb_vorbis in this
// translation unit gives miniaudio seekable OGG decoding without a JS audio
// path, following the TH07/TH08/TH10 web runtimes.
#include "../../../portable/sdl/third_party/stb_vorbis.h"
#define MINIAUDIO_IMPLEMENTATION
#include "../../../portable/sdl/third_party/miniaudio.h"
#include "../platform/Audio.hpp"
#include <eagler/audio/WebSfx.hpp>
namespace web_sfx = eagler::audio::web_sfx;
#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <exception>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace th20::web::audio {
namespace {
std::uint16_t word16(const std::uint8_t* p) { return std::uint16_t(p[0] | (p[1] << 8)); }
std::uint32_t word32(const std::uint8_t* p) { std::uint32_t v; std::memcpy(&v, p, 4); return v; }

struct Host {
    ma_engine engine{};
    SDL_AudioStream* stream = nullptr;
    bool ready = false, paused = false, refilling = true, primed = false;
    static constexpr int chunk = 1024, low = 2048, high = 3072;
    std::uint64_t clock_ms = 1000;
    std::map<std::uint32_t, bool> events;
    std::uint32_t next_event = 1;
    std::vector<SoundBuffer*> watched; // buffers with active notification ranges
    void (*handler)(std::uint32_t, void*) = nullptr;
    void* handler_data = nullptr;
    std::uint32_t pumps = 0, empty_checks = 0, mixed_frames = 0, min_queued = ~0u, error = 0;
    float rms = 0;
} host;

std::uint32_t block_align(const SoundBuffer& b) { return std::max(1u, std::uint32_t(word16(b.format + 12))); }

void controls(SoundBuffer& b) {
    if (!b.voice_ready) return;
    web_sfx::volume(&b, std::pow(10.f, float(b.volume) / 2000.f));
    web_sfx::pan(&b, b.pan >= 0 ? 1.f - std::pow(10.f, -float(b.pan) / 2000.f)
                               : std::pow(10.f, float(b.pan) / 2000.f) - 1.f);
    auto& sound = *static_cast<ma_sound*>(b.voice);
    ma_sound_set_volume(&sound, std::pow(10.f, float(b.volume) / 2000.f));
    ma_sound_set_pan_mode(&sound, ma_pan_mode_balance);
    ma_sound_set_pan(&sound, b.pan >= 0 ? 1.f - std::pow(10.f, -float(b.pan) / 2000.f)
                                        : std::pow(10.f, float(b.pan) / 2000.f) - 1.f);
}

bool prepare_voice(SoundBuffer& b) {
    if (b.voice_ready || (b.flags & 1)) return true; // primary-buffer flag: no voice
    const auto channels = word16(b.format + 2), align = word16(b.format + 12), bits = word16(b.format + 14);
    if (word16(b.format) != 1 || !align || !channels || (bits != 8 && bits != 16)) return false;
    if (!b.data_source) b.data_source = new ma_audio_buffer{};
    if (!b.voice) b.voice = new ma_sound{};
    auto& data = *static_cast<ma_audio_buffer*>(b.data_source);
    auto& sound = *static_cast<ma_sound*>(b.voice);
    if (!b.data_ready) {
        auto cfg = ma_audio_buffer_config_init(bits == 8 ? ma_format_u8 : ma_format_s16,
                                               channels, b.length / align, b.pcm->data(), nullptr);
        cfg.sampleRate = word32(b.format + 4);
        if (ma_audio_buffer_init(&cfg, &data) != MA_SUCCESS) return false;
        b.data_ready = true;
    }
    if (ma_sound_init_from_data_source(&host.engine, &data, MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, &sound) != MA_SUCCESS)
        return false;
    b.voice_ready = true;
    // Mutable music rings retain miniaudio and their original notifications.
    if (b.notifications.empty())
        web_sfx::upload(&b, b.pcm->data(), b.length / align, channels, word32(b.format + 4), bits == 8 ? 2 : 1);
    controls(b);
    return true;
}

void discard_voice(SoundBuffer& b) {
    web_sfx::erase(&b);
    if (b.voice_ready) { ma_sound_uninit(static_cast<ma_sound*>(b.voice)); b.voice_ready = false; }
    if (b.data_ready) { ma_audio_buffer_uninit(static_cast<ma_audio_buffer*>(b.data_source)); b.data_ready = false; }
}

std::uint32_t position_of(SoundBuffer& b) {
    if (!b.playing) return b.cursor;
    if (web_sfx::contains(&b)) {
        if (!web_sfx::playing(&b)) { b.playing = false; return b.cursor = 0; }
        return std::uint32_t(web_sfx::cursor(&b) * block_align(b) % b.length);
    }
    if (b.voice_ready) {
        ma_uint64 frame = 0;
        ma_sound_get_cursor_in_pcm_frames(static_cast<ma_sound*>(b.voice), &frame);
        return std::uint32_t(frame * block_align(b) % b.length);
    }
    const auto pos = b.cursor + std::uint32_t((host.clock_ms - b.started_ms) * b.frequency / 1000) * block_align(b);
    if (b.loop) return pos % b.length;
    if (pos >= b.length) { b.playing = false; return b.cursor = 0; }
    return pos;
}
}

SoundBuffer::~SoundBuffer() {
    auto& watched = host.watched;
    watched.erase(std::remove(watched.begin(), watched.end(), this), watched.end());
    discard_voice(*this);
    delete static_cast<ma_audio_buffer*>(data_source);
    delete static_cast<ma_sound*>(voice);
}

HRESULT SoundBuffer::GetStatus(DWORD* out) {
    if (web_sfx::contains(this)) position_of(*this);
    if (out) *out = playing ? (DSBSTATUS_PLAYING | (loop ? DSBSTATUS_LOOPING : 0u)) : 0u;
    return S_OK;
}
HRESULT SoundBuffer::Restore() { return S_OK; }
HRESULT SoundBuffer::Lock(DWORD offset, DWORD bytes, void** first, DWORD* first_size, void** second, DWORD* second_size, DWORD) {
    offset %= length;
    const auto wanted = bytes ? bytes : length;
    if (wanted > length) return E_INVALIDARG;
    const auto head = std::min(wanted, length - offset);
    *first = pcm->data() + offset;
    *first_size = head;
    if (second) *second = wanted > head ? pcm->data() : nullptr;
    if (second_size) *second_size = wanted - head;
    return S_OK;
}
HRESULT SoundBuffer::Unlock(void*, DWORD, void*, DWORD) {
    if (web_sfx::contains(this)) {
        const bool active = web_sfx::playing(this);
        const auto frame = web_sfx::cursor(this);
        web_sfx::upload(this, pcm->data(), length / block_align(*this), word16(format + 2), word32(format + 4), word16(format + 14) == 8 ? 2 : 1);
        controls(*this);web_sfx::seek(this, frame, word32(format + 4));
        if (active) web_sfx::play(this, loop);
    }
    return S_OK;
}
HRESULT SoundBuffer::Play(DWORD, DWORD, DWORD play_flags) {
    if (!prepare_voice(*this)) return E_FAIL;
    if (playing) cursor = position_of(*this);
    playing = true;
    loop = (play_flags & DSBPLAY_LOOPING) != 0;
    started_ms = host.clock_ms;
    last_cursor = cursor;
    if (voice_ready) {
        auto& sound = *static_cast<ma_sound*>(voice);
        ma_sound_set_looping(&sound, loop);
        ma_sound_seek_to_pcm_frame(&sound, cursor / block_align(*this));
        web_sfx::seek(this, cursor / block_align(*this), word32(format + 4));
        if (!web_sfx::play(this, loop)) ma_sound_start(&sound);
    }
    return S_OK;
}
HRESULT SoundBuffer::Stop() {
    cursor = position_of(*this);
    playing = false;
    web_sfx::stop(this);
    if (voice_ready) ma_sound_stop(static_cast<ma_sound*>(voice));
    return S_OK;
}
HRESULT SoundBuffer::SetCurrentPosition(DWORD byte_position) {
    cursor = byte_position % length;
    web_sfx::seek(this, cursor / block_align(*this), word32(format + 4));
    started_ms = host.clock_ms;
    if (voice_ready) ma_sound_seek_to_pcm_frame(static_cast<ma_sound*>(voice), cursor / block_align(*this));
    return S_OK;
}
HRESULT SoundBuffer::GetCurrentPosition(DWORD* play, DWORD* write) {
    const auto pos = position_of(*this);
    if (play) *play = pos;
    if (write) *write = (pos + 2048) % length;
    return S_OK;
}
HRESULT SoundBuffer::SetVolume(LONG millibels) { volume = millibels; controls(*this); return S_OK; }
HRESULT SoundBuffer::SetPan(LONG millibels) { pan = millibels; controls(*this); return S_OK; }
HRESULT SoundBuffer::SetFormat(const WAVEFORMATEX* wfx) {
    discard_voice(*this);
    std::memcpy(format, wfx, 18);
    frequency = word32(format + 4);
    return S_OK;
}
HRESULT SoundBuffer::SetNotificationPositions(DWORD count, const DSBPOSITIONNOTIFY* positions) {
    if (count) web_sfx::erase(this);
    notifications.clear();
    for (std::uint32_t i = 0; i < count; ++i)
        notifications.emplace_back(positions[i].dwOffset,
            static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(positions[i].hEventNotify)));
    auto& watched = host.watched;
    if (notifications.empty()) watched.erase(std::remove(watched.begin(), watched.end(), this), watched.end());
    else if (std::find(watched.begin(), watched.end(), this) == watched.end()) watched.push_back(this);
    last_cursor = position_of(*this);
    return S_OK;
}
HRESULT SoundBuffer::QueryInterface(REFIID, void** out) {
    if (!out) return E_POINTER;
    *out = this; // one native buffer owns both buffer and notify surfaces
    return S_OK;
}
ULONG SoundBuffer::Release() {
    delete this;
    return 0;
}

bool Device::initialize(std::uint32_t, std::uint32_t rate, std::uint32_t) {
    if (host.ready) return true;
    auto cfg = ma_engine_config_init();
    cfg.noDevice = MA_TRUE;
    cfg.sampleRate = rate;
    cfg.channels = 2;
    // DirectSound changes are immediate; do not add miniaudio's default
    // short gain ramp to the game's own fades.
    cfg.defaultVolumeSmoothTimeInPCMFrames = 0;
    if (ma_engine_init(&cfg, &host.engine) != MA_SUCCESS) { host.error = 1; return false; }
    // SDL Emscripten doubles this request to a 2048-frame output block.
    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES, "1024");
    SDL_AudioSpec spec{SDL_AUDIO_F32, 2, static_cast<int>(std::min<std::uint32_t>(rate, 44100u))};
    if (SDL_InitSubSystem(SDL_INIT_AUDIO))
        host.stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
    if (!host.stream) { ma_engine_uninit(&host.engine); host.error = 2; return false; }
    host.ready = true;
    host.paused = false;
    web_sfx::suspend(false);
    SDL_ResumeAudioStreamDevice(host.stream);
    return true;
}

HRESULT Device::CreateSoundBuffer(const DSBUFFERDESC* description, SoundBuffer** out, IUnknown*) {
    if (!description || !out) return E_INVALIDARG;
    auto b = std::make_unique<SoundBuffer>();
    b->flags = description->dwFlags;
    b->length = description->dwBufferBytes ? description->dwBufferBytes : 4096;
    if (description->lpwfxFormat) std::memcpy(b->format, description->lpwfxFormat, 18);
    else {
        const std::uint8_t pcm16[18]{1, 0, 2, 0, 0x68, 0xac, 0, 0, 0x10, 0xb1, 2, 0, 4, 0, 16, 0, 0, 0};
        std::memcpy(b->format, pcm16, 18);
    }
    b->frequency = word32(b->format + 4);
    b->pcm = std::make_shared<std::vector<std::uint8_t>>(b->length, word16(b->format + 14) == 8 ? 128 : 0);
    *out = b.release();
    return S_OK;
}
HRESULT Device::DuplicateSoundBuffer(SoundBuffer* source, SoundBuffer** out) {
    if (!source || !out) return E_INVALIDARG;
    auto b = std::make_unique<SoundBuffer>();
    b->flags = source->flags;
    b->length = source->length;
    std::memcpy(b->format, source->format, 18);
    b->frequency = source->frequency;
    b->volume = source->volume;
    b->pan = source->pan;
    b->pcm = source->pcm; // clones share storage, cursors stay independent
    *out = b.release();
    return S_OK;
}

Device& device() {
    static Device instance;
    return instance;
}
bool ready() { return host.ready; }

std::uint32_t create_event() { const auto id = host.next_event++; host.events[id] = false; return id; }
void close_event(std::uint32_t id) { host.events.erase(id); }
bool consume_event(std::uint32_t id) {
    auto it = host.events.find(id);
    if (it == host.events.end()) return false;
    const bool signaled = it->second;
    it->second = false;
    return signaled;
}
void set_notification_handler(void (*handler)(std::uint32_t, void*), void* userdata) {
    host.handler = handler;
    host.handler_data = userdata;
}

void advance(std::uint32_t milliseconds) { host.clock_ms += milliseconds; }

namespace {
std::uint32_t render_frames(float* out, std::uint32_t count);
void service_notifications() {
    for (auto* buffer : host.watched) {
        if (!buffer->playing) { buffer->last_cursor = buffer->cursor; continue; }
        const auto pos = position_of(*buffer), last = buffer->last_cursor;
        for (const auto& note : buffer->notifications) {
            const bool crossed = (pos >= last && note.first >= last && note.first < pos) ||
                                 (pos < last && (note.first >= last || note.first < pos));
            if (!crossed) continue;
            auto it = host.events.find(note.second);
            if (it != host.events.end()) it->second = true;
            if (host.handler) host.handler(note.second, host.handler_data);
        }
        buffer->last_cursor = pos;
    }
}
std::uint32_t render_frames(float* out, std::uint32_t count) {
    std::uint32_t done = 0;
    while (done < count) {
        ma_uint64 frames = 0;
        const auto wanted = std::min<std::uint32_t>(Host::chunk, count - done);
        const auto result = ma_engine_read_pcm_frames(&host.engine, out + done * 2, wanted, &frames);
        if (result != MA_SUCCESS && result != MA_AT_END) { host.error = 3; break; }
        if (frames < wanted) {
            std::memset(out + (done + frames) * 2, 0, (wanted - frames) * 2 * sizeof(float));
            frames = wanted;
        }
        done += std::uint32_t(frames);
        // Service refill notifications between mixer slices. A single delayed
        // display callback must not collapse several ring notifications.
        service_notifications();
        if (frames < wanted) break;
    }
    return done;
}
}

void pump() {
    if (!host.ready || host.paused) return;
    int queued = std::max(0, SDL_GetAudioStreamQueued(host.stream)) / 8;
    host.min_queued = std::min(host.min_queued, std::uint32_t(queued));
    if (host.primed && queued == 0) ++host.empty_checks;
    if (!host.refilling && queued < Host::low) host.refilling = true;
    if (!host.refilling) return;
    if (queued >= Host::high) { host.refilling = false; host.primed = true; return; }
    for (int block = 0; block < (Host::high + Host::chunk - 1) / Host::chunk && queued < Host::high; ++block) {
        float pcm[Host::chunk * 2]{};
        const int frames = std::min(Host::chunk, Host::high - queued);
        if (render_frames(pcm, frames) != frames) { host.error = 3; return; }
        double energy = 0;
        for (int i = 0; i < frames * 2; ++i) { auto& sample = pcm[i]; sample = std::clamp(sample, -1.f, 1.f); energy += double(sample) * sample; }
        host.rms = std::sqrt(energy / (frames * 2));
        if (!SDL_PutAudioStreamData(host.stream, pcm, frames * 8)) { host.error = 4; return; }
        ++host.pumps;
        host.mixed_frames += frames;
        queued += frames;
    }
    if (queued >= Host::high) { host.refilling = false; host.primed = true; }
}

void pause(bool paused) {
    web_sfx::suspend(paused);
    if (!host.stream) { host.paused = paused; return; }
    if (host.paused == paused) return;
    host.paused = paused;
    if (paused) SDL_PauseAudioStreamDevice(host.stream);
    else SDL_ResumeAudioStreamDevice(host.stream);
}
void shutdown() {
    if (host.stream) SDL_DestroyAudioStream(host.stream);
    host.stream = nullptr;
    if (host.ready) ma_engine_uninit(&host.engine);
    host.ready = false;
}

const Statistics& statistics() {
    static Statistics out;
    out.ready = host.ready;
    out.events = std::uint32_t(host.events.size());
    out.pumps = host.pumps;
    out.mixed_frames = host.mixed_frames;
    out.queued_frames = host.stream ? std::uint32_t(std::max(0, SDL_GetAudioStreamQueued(host.stream)) / 8) : 0;
    out.empty_checks = host.empty_checks;
    out.min_queued = host.min_queued;
    out.error = host.error;
    out.paused = host.paused;
    out.rms = host.rms;
    out.clock_ms = std::uint32_t(host.clock_ms);
    return out;
}
std::uint32_t render(float* out, std::uint32_t frames) { return host.ready ? render_frames(out, frames) : 0; }
}

// --- JS-facing lifecycle/music controls (shell.mjs protocol) ---------------
namespace th20::web::audio {
namespace {
bool music_enabled = true;
void* active_reader = nullptr; // WaveReader currently streaming, if any
}
bool music_is_enabled() { return music_enabled; }
void track_reader(void* reader) { active_reader = reader; }
void* current_reader() { return active_reader; }
}

// The retail thbgm.dat byte space is presented as a virtual SDL_IOStream whose
// per-track regions are decoded from the canonical /bgm-ogg OGG set. The game
// keeps its own thbgm.fmt archive offsets, loop points and refill worker; only
// the storage behind those byte ranges changes. Mirrors TH07/TH08/TH10.
namespace {
bool music_stream_enabled() { return th20::web::audio::music_is_enabled(); }
struct MusicEntry { std::uint32_t offset, length; const char* file; };
// thbgm.fmt order (archive_offset ascending). The final track is a TH128 borrow
// shipped under its original name; every other track keeps its th20_XX name.
constexpr MusicEntry music_entries[] = {
    {16u, 17292480u, "th20_01.ogg"},
    {17292496u, 23001472u, "th20_02.ogg"},
    {40293968u, 17432128u, "th20_03.ogg"},
    {57726096u, 24779200u, "th20_04.ogg"},
    {82505296u, 24280896u, "th20_05.ogg"},
    {106786192u, 25786592u, "th20_06.ogg"},
    {132572784u, 23677952u, "th20_07.ogg"},
    {156250736u, 29884160u, "th20_08.ogg"},
    {186134896u, 31100672u, "th20_09.ogg"},
    {217235568u, 31494144u, "th20_10.ogg"},
    {248729712u, 26837376u, "th20_11.ogg"},
    {275567088u, 17296768u, "th20_13.ogg"},
    {292863856u, 34336192u, "th20_12.ogg"},
    {327200048u, 32871936u, "th20_17.ogg"},
    {360071984u, 31570240u, "th20_18.ogg"},
    {391642224u, 8438912u, "th20_15.ogg"},
    {400081136u, 12913664u, "th20_14.ogg"},
    {412994800u, 10054800u, "th20_16.ogg"},
    {423049600u, 6741184u, "th128_08.ogg"},
};
constexpr std::size_t music_track_count = sizeof(music_entries) / sizeof(music_entries[0]);
constexpr std::uint64_t music_size = std::uint64_t(music_entries[music_track_count - 1].offset) + music_entries[music_track_count - 1].length;
bool ogg_full = false;
struct MusicFile;
std::vector<MusicFile*> music_streams;
struct MusicFile {
    std::uint64_t cursor = 0, decoder_frame = 0;
    int track = -1;
    SDL_IOStream* source = nullptr;
    ma_decoder decoder{};
    std::vector<std::uint8_t> full_pcm;
    bool valid = false, full_ready = false, waiting = false;
    ~MusicFile() { music_streams.erase(std::remove(music_streams.begin(), music_streams.end(), this), music_streams.end()); reset(); }
    void reset() {
        if (valid) ma_decoder_uninit(&decoder);
        if (source) SDL_CloseIO(source);
        source = nullptr; valid = false; full_ready = false; waiting = false; track = -1; full_pcm.clear();
    }
    static ma_result read(ma_decoder* d, void* out, std::size_t size, std::size_t* read) {
        *read = SDL_ReadIO(static_cast<MusicFile*>(d->pUserData)->source, out, size);
        return *read ? MA_SUCCESS : MA_AT_END;
    }
    static ma_result seek(ma_decoder* d, ma_int64 offset, ma_seek_origin origin) {
        return SDL_SeekIO(static_cast<MusicFile*>(d->pUserData)->source, offset,
                          origin == ma_seek_origin_start ? SDL_IO_SEEK_SET
                          : origin == ma_seek_origin_current ? SDL_IO_SEEK_CUR : SDL_IO_SEEK_END) >= 0 ? MA_SUCCESS : MA_BAD_SEEK;
    }
    bool select(int n, const MusicEntry& entry) {
        if (n < 0 || n >= int(music_track_count)) return false;
        if (track == n && (valid || full_ready)) return true;
        reset(); track = n;
        char name[64];
        std::snprintf(name, sizeof(name), "/bgm-ogg/%s", entry.file);
        source = SDL_IOFromFile(name, "rb");
        if (!source) { waiting = true; return false; }
        auto cfg = ma_decoder_config_init(ma_format_s16, 2, 44100);
        if (ma_decoder_init(read, seek, this, &cfg, &decoder) != MA_SUCCESS) { reset(); return false; }
        valid = true; decoder_frame = 0;
        if (!ogg_full) return true;
        const auto frames = entry.length / 4;
        full_pcm.resize(std::size_t(entry.length));
        std::uint64_t decoded = 0;
        while (decoded < frames) {
            ma_uint64 actual = 0;
            const auto result = ma_decoder_read_pcm_frames(&decoder, full_pcm.data() + std::size_t(decoded) * 4,
                                                           std::min<std::uint64_t>(4096, frames - decoded), &actual);
            if ((result != MA_SUCCESS && result != MA_AT_END) || !actual) { reset(); return false; }
            decoded += actual;
        }
        ma_decoder_uninit(&decoder); valid = false; SDL_CloseIO(source); source = nullptr;
        full_ready = true; waiting = false; return true;
    }
    void resource_changed() { if (waiting) { const auto n = track; reset(); track = n; } }
};
Sint64 musicSize(void*) { return Sint64(music_size); }
Sint64 musicSeek(void* user, Sint64 off, SDL_IOWhence origin) {
    auto& f = *static_cast<MusicFile*>(user);
    const auto pos = (origin == SDL_IO_SEEK_SET ? Sint64(0) : origin == SDL_IO_SEEK_CUR ? Sint64(f.cursor) : Sint64(music_size)) + off;
    if (pos < 0) return -1;
    f.cursor = std::uint64_t(pos); return pos;
}
std::size_t musicRead(void* user, void* out, std::size_t size, SDL_IOStatus* status) {
    auto& f = *static_cast<MusicFile*>(user);
    auto* dest = static_cast<std::uint8_t*>(out);
    std::size_t done = 0;
    *status = SDL_IO_STATUS_READY;
    if (!music_stream_enabled()) {
        const auto count = std::min<std::uint64_t>(size, f.cursor < music_size ? music_size - f.cursor : 0);
        std::memset(out, 0, count); f.cursor += count;
        if (f.cursor >= music_size) *status = SDL_IO_STATUS_EOF;
        return count;
    }
    while (done < size && f.cursor < music_size) {
        if (f.cursor < 16) {
            const auto n = std::min<std::uint64_t>(size - done, 16 - f.cursor);
            std::memset(dest + done, 0, n); done += n; f.cursor += n; continue;
        }
        int n = int(music_track_count) - 1;
        while (n > 0 && f.cursor < music_entries[n].offset) --n;
        const auto& t = music_entries[n];
        if (!f.select(n, t)) {
            // A managed OGG may arrive after the Runtime starts. Do not turn a
            // missing optional component into an IO error; the resource-change
            // export invalidates this pending selection for retry.
            const auto count = std::min<std::uint64_t>(size - done, t.length - (f.cursor - t.offset));
            std::memset(dest + done, 0, count); done += count; f.cursor += count; continue;
        }
        const auto local = f.cursor - t.offset, frame = local / 4;
        const std::size_t count = std::size_t(std::min<std::uint64_t>(size - done, t.length - local)), leading = local % 4;
        if (f.full_ready) { std::memcpy(dest + done, f.full_pcm.data() + local, count); f.cursor += count; done += count; continue; }
        if (frame != f.decoder_frame && ma_decoder_seek_to_pcm_frame(&f.decoder, frame) != MA_SUCCESS) { *status = SDL_IO_STATUS_ERROR; break; }
        ma_uint64 actual = 0; const auto frames = (count + leading + 3) / 4;
        std::vector<std::uint8_t> scratch; void* target = dest + done;
        if (leading || count % 4) { scratch.resize(frames * 4); target = scratch.data(); }
        const auto result = ma_decoder_read_pcm_frames(&f.decoder, target, frames, &actual);
        f.decoder_frame = frame + actual;
        const auto bytes = std::min<std::uint64_t>(count, actual * 4 > leading ? actual * 4 - leading : 0);
        if ((result != MA_SUCCESS && result != MA_AT_END) || !bytes) { *status = SDL_IO_STATUS_ERROR; break; }
        if (!scratch.empty()) std::memcpy(dest + done, scratch.data() + leading, bytes);
        f.cursor += bytes; done += bytes;
    }
    if (f.cursor >= music_size) *status = SDL_IO_STATUS_EOF;
    return done;
}
bool musicClose(void* user) { delete static_cast<MusicFile*>(user); return true; }
}
extern "C" SDL_IOStream* th20_music_stream() {
    SDL_IOStreamInterface iface{};
    SDL_INIT_INTERFACE(&iface);
    iface.size = musicSize; iface.seek = musicSeek; iface.read = musicRead; iface.close = musicClose;
    auto* f = new MusicFile();
    music_streams.push_back(f);
    auto* io = SDL_OpenIO(&iface, f);
    if (!io) { music_streams.pop_back(); delete f; }
    return io;
}

extern "C" {
__attribute__((export_name("sdl_audio_pump"))) void sdl_audio_pump() try { th20::web::audio::pump(); }
catch (const std::exception& e) { std::fprintf(stderr, "sdl_audio_pump: exception: %s\n", e.what()); }
catch (...) { std::fprintf(stderr, "sdl_audio_pump: unknown exception\n"); }
__attribute__((export_name("sdl_audio_pause"))) void sdl_audio_pause(std::uint32_t paused) { th20::web::audio::pause(paused != 0); }
__attribute__((export_name("sdl_audio_shutdown"))) void sdl_audio_shutdown() { th20::web::audio::shutdown(); }
__attribute__((export_name("sdl_audio_stats"))) const th20::web::audio::Statistics* sdl_audio_stats() { return &th20::web::audio::statistics(); }
__attribute__((export_name("sdl_audio_render"))) std::uint32_t sdl_audio_render(float* out, std::uint32_t frames) try { return th20::web::audio::render(out, frames); }
catch (const std::exception& e) { std::fprintf(stderr, "sdl_audio_render: exception: %s\n", e.what()); return 0; }
catch (...) { std::fprintf(stderr, "sdl_audio_render: unknown exception\n"); return 0; }
__attribute__((export_name("sdl_audio_event"))) std::uint32_t sdl_audio_event(std::uint32_t op, std::uint32_t id) {
    if (op == 0) return th20::web::audio::create_event();
    if (op == 2) { th20::web::audio::close_event(id); return 0; }
    return th20::web::audio::consume_event(id) ? 0 : 0x102;
}
// BGM enable follows the TH07/TH10 protocol: music keeps its timing and stream
// state, but the PCM the game reads becomes silence.
__attribute__((export_name("sdl_music_enabled"))) void sdl_music_enabled(std::uint32_t enabled) { th20::web::audio::music_enabled = enabled != 0; }
__attribute__((export_name("sdl_music_resource_changed"))) void sdl_music_resource_changed() {
    // A late-arriving OGG invalidates pending selections so the next read
    // retries against the fresh MEMFS bytes.
    for (auto* stream : music_streams) stream->resource_changed();
}
__attribute__((export_name("sdl_music_stats"))) const std::uint32_t* sdl_music_stats() {
    static std::uint32_t out[6]{};
    out[0] = th20::web::audio::music_is_enabled();
    out[1] = ogg_full ? 1u : 0u;
    out[2] = std::uint32_t(music_streams.size());
    out[3] = out[4] = out[5] = 0;
    for (auto* stream : music_streams) { out[3] += stream->waiting; out[4] += stream->valid; out[5] += stream->full_ready; }
    return out;
}
__attribute__((export_name("sdl_ogg_decode_mode"))) void sdl_ogg_decode_mode(std::uint32_t full) { ogg_full = full != 0; }
}
