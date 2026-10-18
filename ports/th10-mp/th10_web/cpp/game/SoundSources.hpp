#pragma once
#include "AudioManager.hpp"
#include "SoundResources.hpp"
namespace th10 {
struct SoundSourceEnvironment {
    SoundResourceEnvironment* resources;
    const i32* load_stop;
    virtual void sleep(u32 milliseconds)=0;
    virtual void free_source(void* bytes)=0;
    virtual void unlock_source(void* buffer,const SoundLock& lock)=0;
    virtual void invalid_source(bool riff,const char* filename)=0;
};
struct SoundSources {
    AudioManager& manager;SoundSourceEnvironment& environment;
    i32 load(i32 index,const char* filename);
    bool begin(i32 index);
    i32 complete(i32 index,const char* filename);
    static const u8* find_chunk(const u8* begin,u32 bytes,const char* name,u32& length);
};
struct SoundSourceLoad {
    i32 index=0,result=0;const char* filename=nullptr;bool started=false,waiting=false,done=false;
    bool advance(AudioManager& manager,SoundSourceEnvironment& environment);
};
}
