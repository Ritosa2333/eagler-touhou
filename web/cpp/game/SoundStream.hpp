#pragma once
#include "SoundBuffer.hpp"
namespace th10 {
struct SoundBufferDescription {u32 size,flags,bytes,reserved;void* format;u8 algorithm[16];};
struct SoundStream : SoundBuffer {
    SoundBufferDescription description;
    void** device;
    u32 previous_play_cursor,played_bytes,write_cursor,silent,chunk_bytes;
    i32 update(bool loop,SoundBufferEnvironment& environment);
    i32 reset(SoundBufferEnvironment& environment);
};
static_assert(sizeof(SoundStream)==0x70);
struct NotifiedSoundStream : SoundStream {u32 notification_event,notification_busy;};
static_assert(sizeof(NotifiedSoundStream)==0x78);
}
