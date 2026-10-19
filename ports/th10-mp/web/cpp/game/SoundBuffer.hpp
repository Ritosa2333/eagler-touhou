#pragma once
#include "Arithmetic.hpp"
#include "WaveReader.hpp"
namespace th10 {
struct SoundBufferEnvironment;
struct SoundLock {u8* first;u32 first_bytes;u8* second;u32 second_bytes;};
struct SoundBuffer {
    u32 virtual_table;
    void** buffers;
    u32 buffer_bytes;
    WaveReader* wave;
    u32 buffer_count;
    i32 fade_remaining,fade_duration,fade_mode;
    u32 priority,play_flags,reserved_028,play_position,active;
    void* buffer(u32 index) const noexcept;
    void* available(SoundBufferEnvironment& environment);
    u32 playing(SoundBufferEnvironment& environment);
    i32 set_volume(i32 adjustment,SoundBufferEnvironment& environment);
    i32 play(u32 priority,u32 flags,SoundBufferEnvironment& environment);
    i32 stop_all(SoundBufferEnvironment& environment);
    i32 stop_first(SoundBufferEnvironment& environment);
    i32 resume(SoundBufferEnvironment& environment);
    i32 rewind(SoundBufferEnvironment& environment);
    i32 query(u32 index,void** result,SoundBufferEnvironment& environment);
    i32 fill(void* buffer,bool repeat,SoundBufferEnvironment& environment);
    static i32 restore(void* buffer,u32* restored,SoundBufferEnvironment& environment);
    static void advance_fades(SoundBuffer** current,SoundBufferEnvironment& environment);
    i32 advance_fade(i32 mode,SoundBufferEnvironment& environment);
};
static_assert(sizeof(SoundBuffer)==0x34&&offsetof(SoundBuffer,fade_mode)==0x1c);
struct SoundBufferEnvironment {
    const i32* music_volume;
    WaveEnvironment* waves;
    virtual i32 status(void* buffer,u32* status)=0;
    virtual i32 restore(void* buffer)=0;
    virtual void sleep(u32 milliseconds)=0;
    virtual u32 random_buffer()=0;
    virtual i32 volume(void* buffer,i32 gain)=0;
    virtual i32 play(void* buffer,u32 priority,u32 flags)=0;
    virtual i32 stop(void* buffer)=0;
    virtual i32 position(void* buffer,u32 offset)=0;
    virtual i32 refill(SoundBuffer& owner,void* buffer,bool repeat)=0;
    virtual i32 query(void* buffer,void** result)=0;
    virtual i32 lock(void* buffer,u32 offset,u32 bytes,SoundLock& area,bool split)=0;
    virtual i32 unlock(void* buffer,const SoundLock& area)=0;
    virtual i32 current_position(void* buffer,u32* play,u32* write)=0;
};
}
