#pragma once
#include "FileSystem.hpp"
#include "../game/AudioWorkers.hpp"
#include "../game/AudioControl.hpp"
#include "../game/AudioResources.hpp"
#include <initializer_list>
namespace th10::browser {
enum class AudioOperation:u32 {AddRef,Release,CreateBuffer,Duplicate,Cooperative,Format,Status,Restore,Volume,Pan,Play,Stop,Position,CurrentPosition,Lock,Unlock,Query,Notifications,Frequency,GetVolume,GetPan,GetFrequency,GetFormat,Caps};
struct AudioHost {
    virtual u32 create()=0;
    virtual i32 call(u32 handle,AudioOperation operation,const u32* arguments)=0;
    virtual u32 event(u32 operation,u32 handle)=0;
    virtual void advance(u32 milliseconds)=0;
};
struct Audio;
struct AudioWaves final:WaveEnvironment {
    Audio& owner;explicit AudioWaves(Audio&);
    u32 open_file(const char*) override;
    void seek_file(u32,u32) override;
    void read_file(u32,u8*,u32,u32*) override;
    void close_file(u32) override;
};
struct AudioBuffers final:SoundBufferEnvironment {
    Audio& owner;explicit AudioBuffers(Audio&);
    i32 status(void*,u32*) override;i32 restore(void*) override;void sleep(u32) override;
    u32 random_buffer() override;i32 volume(void*,i32) override;i32 play(void*,u32,u32) override;
    i32 stop(void*) override;i32 position(void*,u32) override;
    i32 refill(SoundBuffer&,void*,bool) override;i32 query(void*,void**) override;
    i32 lock(void*,u32,u32,SoundLock&,bool) override;i32 unlock(void*,const SoundLock&) override;
    i32 current_position(void*,u32*,u32*) override;
};
struct AudioSounds final:SoundResourceEnvironment {
    Audio& owner;explicit AudioSounds(Audio&);
    void* allocate(u32) override;void free(void*) override;void release(void*) override;
    i32 create_buffer(void*,const SoundBufferDescription&,void**) override;
    i32 query_notifications(void*,void**) override;i32 set_notifications(void*,u32,const SoundNotification*) override;
};
struct AudioControls final:AudioControlEnvironment {
    Audio& owner;explicit AudioControls(Audio&);
    void prepare_track(AudioManager&,i32,const char*) override;i32 start_track(AudioManager&,i32) override;
    void stop_music(NotifiedSoundStream&) override;i32 reset_music(NotifiedSoundStream&) override;
    i32 fill_music(NotifiedSoundStream&,void*,bool) override;void recreate_music(NotifiedSoundStream&) override;
    void select_music(WaveReader&,const MusicFormat*) override;void play_music(NotifiedSoundStream&) override;
    void pause_music(NotifiedSoundStream&) override;void resume_music(NotifiedSoundStream&) override;
    void volume_music(NotifiedSoundStream&,i32) override;void delete_music(NotifiedSoundStream&) override;
    void quit_thread(u32) override;u32 wait_thread(u32,u32) override;void close_handle(u32) override;
    void effect_stop(void*) override;void effect_position(void*,u32) override;void effect_pan(void*,i32) override;
    void effect_volume(void*,i32) override;void effect_play(void*) override;
};
struct AudioResourcesHost final:AudioResourceEnvironment {
    Audio& owner;explicit AudioResourcesHost(Audio&);
    void unload_music(AudioManager&) override;i32 select_music(AudioManager&,const char*) override;
    i32 track_index(AudioManager&,const char*) override;u8* allocate_cache(u32) override;void free_cache(u8*) override;
    u32 create_event() override;u32 create_worker(u32,u32*) override;
    i32 create_file_stream(void*,NotifiedSoundStream**,const char*,const MusicFormat*,u32,u32) override;
    i32 create_memory_stream(void*,NotifiedSoundStream**,const u8*,u32,const MusicFormat*,u32,u32) override;
};
struct AudioDevices final:AudioDeviceEnvironment {
    Audio& owner;explicit AudioDevices(Audio&);
    void sleep(u32) override;void free_source(void*) override;void unlock_source(void*,const SoundLock&) override;
    void invalid_source(bool,const char*) override;
    u32 begin_thread(CallbackToken,void*,u32,u32&) override;u32 wait_thread(u32,u32) override;void close_thread(u32) override;
    void join_loaders(AudioManager&) override;void unload_music(AudioManager&) override;void delete_music(NotifiedSoundStream&) override;
    i32 create_device(void**) override;i32 cooperative_level(void*,u32) override;i32 set_format(void*,const u8*) override;
    void duplicate_buffer(void*,void*,void**) override;void begin_timer(u32) override;void end_timer(u32) override;
    i32 load_source(AudioManager&,i32,const char*) override;void log_device(u32,const char* =nullptr) override;
};
struct AudioWorkerEnvironment final:AudioSourceLoadingEnvironment,MusicNotificationEnvironment {
    Audio& owner;explicit AudioWorkerEnvironment(Audio&);
    u8* read_wave(const char*) override;void missing_wave(const char*) override;
    bool next_message(u32&) override;void fill_stream(NotifiedSoundStream&) override;
};
enum class AudioTaskKind:u32 {Device=1,Sources=2,Music=3};
struct AudioTask {
    u32 id=0;AudioTaskKind kind=AudioTaskKind::Device;bool done=false,quit=false;
    AudioDeviceWorkerTask device;AudioSourceLoadingTask sources;MusicNotificationTask music;
};
// Cooperative native tasks. No saved CPU registers, original function addresses
// or guest stacks are involved. Task creation never reenters the creating call.
struct Audio {
    AudioHost& host;FileSystem& files;AudioManager manager{};
    u32 display_flags=0,window=0,random_state=1,next_task=0x10000;
    u8 music_enabled=1,effects_enabled=1;std::int8_t configured_music=100,configured_effects=100;
    i32 error=0;bool timer_active=false,pumping=false;AudioTask* active_task=nullptr;AudioTask tasks[8]{};
    AudioWaves waves;AudioBuffers buffers;AudioSounds sounds;AudioControls controls;
    AudioResourcesHost resources;AudioDevices devices;AudioWorkerEnvironment workers;
    Audio(AudioHost&,FileSystem&);~Audio();
    i32 call(void*,AudioOperation,std::initializer_list<u32> args={});
    u32 create_task(AudioTaskKind);AudioTask* task(u32);void pump();
    u32 wait_task(u32);void close_task(u32);void advance(u32);
    i32 load_formats(const char*);i32 begin_loading(u32 window);i32 finish_loading();i32 initialize(u32 window);void release();
    i32 update();void advance_fades();
};
}
