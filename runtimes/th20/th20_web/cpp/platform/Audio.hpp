// Software audio for the TH_SDL3 (wasm) build, replacing DirectSound.
// SoundBuffer/Device are REAL software implementations (PCM buffers mixed by
// a miniaudio engine with no device IO, pumped into an SDL audio stream by
// the frame loop). The method surface mirrors the original's usage because
// the recovered game logic is written against that shape; every method
// performs the actual operation. There is no notification thread, keep-alive
// silence buffer or service timer: refills are driven by the mixer crossing
// notification offsets and invoking the registered handler on the main
// thread.
#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <dsound.h>

namespace th20::web::audio {

class SoundBuffer {
public:
    SoundBuffer() = default;
public:
    ~SoundBuffer();
    SoundBuffer(const SoundBuffer&) = delete;
    SoundBuffer& operator=(const SoundBuffer&) = delete;

    HRESULT GetStatus(DWORD*);
    HRESULT Restore();                        // software buffers are never lost
    HRESULT Lock(DWORD offset, DWORD bytes, void**, DWORD*, void**, DWORD*, DWORD flags);
    HRESULT Unlock(void*, DWORD, void*, DWORD);
    HRESULT Play(DWORD reserved, DWORD priority, DWORD flags); // DSBPLAY_LOOPING
    HRESULT Stop();
    HRESULT SetCurrentPosition(DWORD byte_position);
    HRESULT GetCurrentPosition(DWORD* play, DWORD* write);
    HRESULT SetVolume(LONG millibels);        // DirectSound 100ths of a dB
    HRESULT SetPan(LONG millibels);
    HRESULT SetFormat(const WAVEFORMATEX*);
    HRESULT SetNotificationPositions(DWORD count, const DSBPOSITIONNOTIFY*);
    HRESULT QueryInterface(REFIID, void**);
    ULONG Release();                          // destroys the buffer (final ref)

    // Host internals (not used by game code).
    std::uint32_t flags = 0, length = 0, frequency = 44100;
    std::int32_t volume = 0, pan = 0;
    bool playing = false, loop = false;
    std::uint32_t cursor = 0, last_cursor = 0;
    std::uint64_t started_ms = 0;
    std::uint8_t format[18]{};
    std::shared_ptr<std::vector<std::uint8_t>> pcm;
    std::vector<std::pair<std::uint32_t, std::uint32_t>> notifications; // offset, event
    void* data_source = nullptr;   // ma_audio_buffer*
    void* voice = nullptr;         // ma_sound*
    bool data_ready = false, voice_ready = false;
};

class Device {
public:
    bool initialize(std::uint32_t channels, std::uint32_t rate, std::uint32_t bits);
    HRESULT CreateSoundBuffer(const DSBUFFERDESC*, SoundBuffer**, IUnknown*);
    HRESULT DuplicateSoundBuffer(SoundBuffer*, SoundBuffer**);
    HRESULT SetCooperativeLevel(HWND, DWORD) { return S_OK; } // sole audio owner
    ULONG Release() { return 1; }                             // host-owned singleton
};

Device& device();                 // the single shared device/mixer
bool ready();

// Event flags (CreateEventW/SetEvent/WaitForSingleObject/CloseHandle roles).
std::uint32_t create_event();
void close_event(std::uint32_t);
bool consume_event(std::uint32_t); // returns and clears the signaled flag

// Mixer-side notification hook: called on the main thread between mixer
// chunks when a notification offset is crossed (the refill worker role).
void set_notification_handler(void (*handler)(std::uint32_t event, void*), void* userdata);

void advance(std::uint32_t milliseconds);  // audio clock (wall fallback for cursors)
void pump();                               // refill the SDL stream (frame loop)
void pause(bool);
void shutdown();

struct Statistics {
    std::uint32_t ready = 0, buffers = 0, events = 0, pumps = 0, mixed_frames = 0,
        queued_frames = 0, empty_checks = 0, min_queued = ~0u, error = 0, paused = 0;
    float rms = 0;
    std::uint32_t clock_ms = 0;
};
const Statistics& statistics();
std::uint32_t render(float* out, std::uint32_t frames); // deterministic mixer probe
}
