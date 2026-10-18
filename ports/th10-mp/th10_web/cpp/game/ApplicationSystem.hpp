#pragma once
#include "ApplicationState.hpp"
#include "AudioManager.hpp"
#include "Rng.hpp"
namespace th10 {
struct MultimediaTimerEnvironment {
    virtual void kill_timer(u32 id)=0;
    virtual void end_period(u32 period)=0;
};
struct MultimediaTimer {
    u32 original_virtual_table,id,period;
    void stop(MultimediaTimerEnvironment& environment);
    void release(MultimediaTimerEnvironment& environment);
};
struct ApplicationSystemEnvironment : MultimediaTimerEnvironment {
    ApplicationState* global;
    AnmManager** animations;AudioManager* audio;
    BackgroundThread* input_worker;BackgroundThreadEnvironment* threads;
    MultimediaTimer** timer;
    u8** version_data;u32* version_size;
    float* rate;u32* initial_time;Rng* script_random;Rng* visual_random;
    UpdateChain** chain;UpdateChainEnvironment* callbacks;
    CallbackToken initialize_callback,update_callback,draw_callbacks[3],input_callback;
    const char* archive_name;const char* version_name;const char* stop_music_name;
    virtual bool open_archive(const char* name)=0;
    virtual u8* read_version(const char* name,u32& bytes)=0;
    virtual void report_archive_error(bool version)=0;
    virtual void free_bytes(void* bytes)=0;
    virtual void release_object(void* object)=0;
    virtual u32 milliseconds()=0;
    virtual void begin_audio_loading()=0;
    virtual void create_statistics()=0;
    virtual void destroy_statistics()=0;
    virtual void initialize_model(AnmManager& animations)=0;
    virtual void initialize_fonts()=0;
    virtual void release_fonts()=0;
    virtual void shutdown_screens()=0;
    virtual void unacquire(void* device)=0;
    virtual void release_device(void* device)=0;
    virtual void release_archive()=0;
};
struct ApplicationSystem {
    ApplicationSystemEnvironment& environment;
    i32 open_resources();
    i32 initialize();
    void restart_input();
    i32 install_callbacks();
    i32 shutdown(ApplicationState& application);
};
}
