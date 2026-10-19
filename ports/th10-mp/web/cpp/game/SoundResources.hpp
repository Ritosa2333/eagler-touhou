#pragma once
#include "SoundStream.hpp"
namespace th10 {
struct SoundNotification {u32 offset,event;};
struct SoundResourceEnvironment {
    SoundBufferEnvironment* audio;
    u32 buffer_vtable,stream_vtable;
    virtual void* allocate(u32 bytes)=0;
    virtual void free(void* bytes)=0;
    virtual void release(void* object)=0;
    virtual i32 create_buffer(void* device,const SoundBufferDescription& description,void** result)=0;
    virtual i32 query_notifications(void* buffer,void** result)=0;
    virtual i32 set_notifications(void* object,u32 count,const SoundNotification* positions)=0;
};
struct SoundResources {
    SoundResourceEnvironment& environment;
    void initialize(SoundBuffer& value,void* const* buffers,u32 count,u32 bytes,WaveReader* wave);
    void initialize_stream(SoundStream& value,void* buffer,u32 bytes,WaveReader* wave,u32 chunk);
    void release(SoundBuffer& value);
    i32 recreate_notifications(NotifiedSoundStream& value);
    i32 create_file_stream(NotifiedSoundStream** output,void** driver,u32 flags,const char* filename,const u8* algorithm,u32 notifications,u32 chunk,u32 event,const MusicFormat* format);
    i32 create_memory_stream(NotifiedSoundStream** output,void** driver,u32 flags,const u8* data,u32 bytes,const MusicFormat* format,const u8* algorithm,u32 notifications,u32 chunk,u32 event);
private:
    void release_buffers(SoundBuffer& value);
    i32 create_stream(NotifiedSoundStream** output,void** driver,u32 flags,WaveReader* wave,const u8* algorithm,u32 notifications,u32 chunk,u32 event);
};
}
