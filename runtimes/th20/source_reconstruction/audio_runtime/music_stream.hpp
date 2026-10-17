#pragma once
#include "wave_reader.hpp"
#include <atomic>

namespace th20::source::audio {
#pragma pack(push,4)
class MusicStream {
public:
    virtual ~MusicStream();                         // 0x4597b0/stream derived destructor
    virtual HRESULT reset(std::uint32_t);           // 0x45b490
    std::uint32_t retained_04;
    SoundBuffer** buffers;                   // +8
    std::uint32_t buffer_size;                      // +c
    WaveReader* wave;                              // +10
    std::uint32_t buffer_count;                     // +14
    std::int32_t fade_remaining,fade_duration,fade_mode;
    DWORD play_priority,play_flags;                 // +24/+28
    std::uint32_t retained_2c,retained_30,retained_34;
    double started_at,paused_at,paused_duration,retained_50;
    std::uint32_t playing,paused;                   // +58/+5c
    DSBUFFERDESC description;                      // +60
    DeviceOwner* owner;                            // +84
    std::uint32_t last_play_cursor,played_bytes,next_write,retained_94,silence,notification_size;
    HANDLE notification_event;                     // +a0
    std::atomic<std::uint32_t> busy;                // +a4
    SoundInf* sound;                               // source-only +a8

    MusicStream(SoundInf&,SoundBuffer*,std::uint32_t,WaveReader*,std::uint32_t);
    HRESULT restore(SoundBuffer*,BOOL*);     // 0x45b780
    HRESULT fill(SoundBuffer*,bool,std::uint32_t); // 0x45a240
    SoundBuffer* buffer(unsigned) const;     // 0x45a460
    SoundBuffer* free_buffer();              // 0x45a4a0
    HRESULT reset_all_positions();                 // 0x45b410
    HRESULT handle_notification(bool);             // 0x45a580
    HRESULT stop(bool);                            // 0x45bb00
    HRESULT play(DWORD,DWORD,std::uint32_t);        // 0x45add0
    HRESULT pause();                              // 0x45acf0
    HRESULT resume();                             // 0x45bbe0
    HRESULT reopen(TrackFormat*,std::uint32_t);     // 0x45af10
    HRESULT recreate(TrackFormat*);                // 0x45b150
    HRESULT set_volume(std::int32_t);              // 0x45b9b0
    void fade_out(float);                          // 0x4262f0
    int tick_fade(unsigned mode);                  // 0x45a050/0c0/130/1a0
    double playback_seconds();                     // 0x45bd10
    HRESULT seek_seconds(double);                 // 0x45beb0
    HRESULT replace_track(TrackFormat*);           // 0x45bab0
};
#pragma pack(pop)
static_assert(offsetof(MusicStream,description)==0x60);
static_assert(offsetof(MusicStream,busy)==0xa4 && offsetof(MusicStream,sound)==0xa8);
HRESULT create_music_stream(SoundInf&,WaveReader*,DWORD,const GUID&,unsigned,std::uint32_t,HANDLE);
void update_stream(SoundInf&);                      // 0x4d9710
}
