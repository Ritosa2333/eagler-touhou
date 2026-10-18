#pragma once
#include "SoundSources.hpp"
#include "BackgroundThread.hpp"
namespace th10 {
struct AudioDeviceEnvironment : SoundSourceEnvironment,BackgroundThreadEnvironment {
    AudioManager* global_manager;
    const SoundDefinition* definitions;
    const std::int8_t *configured_music,*configured_effects;
    const char* const* source_names;
    CallbackToken device_worker_callback,source_worker_callback;
    virtual void sleep(u32 milliseconds)=0;
    virtual void join_loaders(AudioManager& manager)=0;
    virtual void unload_music(AudioManager& manager)=0;
    virtual void delete_music(NotifiedSoundStream& music)=0;
    virtual i32 create_device(void** output)=0;
    virtual i32 cooperative_level(void* device,u32 window)=0;
    virtual i32 set_format(void* buffer,const u8* format)=0;
    virtual void duplicate_buffer(void* device,void* source,void** output)=0;
    virtual void begin_timer(u32 window)=0;
    virtual void end_timer(u32 window)=0;
    virtual i32 load_source(AudioManager& manager,i32 index,const char* name)=0;
    virtual void log_device(u32 message,const char* filename=nullptr)=0;
};
struct AudioDevice {
    AudioManager& manager;AudioDeviceEnvironment& environment;
    void begin_loading(u32 window);
    void begin_source_loading();
    void join_loading();
    i32 initialize(u32 window);
    i32 initialize_hardware(u32 window);
    i32 initialize_effects();
    void release();
    i32 configure_primary(void** driver,u32 channels,u32 rate,u32 bits);
    void release_driver(void** driver);
};
struct AudioInitializationTask {
    i32 index=0,result=0;bool started=false,done=false;SoundSourceLoad source;
    bool advance(AudioManager& manager,u32 window,AudioDeviceEnvironment& environment);
};
}
