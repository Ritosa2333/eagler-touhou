#pragma once
#include "AudioManager.hpp"
namespace th10 {
struct AudioResourceEnvironment {
    const u32 *display_flags,*worker_argument;
    const u8* music_enabled;
    char (*global_music_names)[256];
    WaveEnvironment* files;
    virtual void unload_music(AudioManager& manager)=0;
    virtual i32 select_music(AudioManager& manager,const char* name)=0;
    virtual i32 track_index(AudioManager& manager,const char* name)=0;
    virtual u8* allocate_cache(u32 bytes)=0;
    virtual void free_cache(u8* bytes)=0;
    virtual u32 create_event()=0;
    virtual u32 create_worker(u32 argument,u32* thread_id)=0;
    virtual i32 create_file_stream(void* driver,NotifiedSoundStream** output,const char* filename,const MusicFormat* format,u32 chunk,u32 event)=0;
    virtual i32 create_memory_stream(void* driver,NotifiedSoundStream** output,const u8* data,u32 bytes,const MusicFormat* format,u32 chunk,u32 event)=0;
};
struct AudioResources {
    AudioManager& manager;AudioResourceEnvironment& environment;
    i32 start_file_music(const char* filename);
    i32 prepare_track(i32 index,const char* name);
    i32 start_track(i32 index);
private:
    u32 start_worker(const MusicFormat& format);
};
}
