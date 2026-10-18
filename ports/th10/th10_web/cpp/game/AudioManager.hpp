#pragma once
#include "SoundStream.hpp"
namespace th10 {
struct SoundDefinition {i32 source;std::int16_t volume,lifetime;};
struct MusicCommand {i32 kind,argument,step;char filename[256];};
static_assert(sizeof(SoundDefinition)==8&&sizeof(MusicCommand)==0x10c);
struct AudioManager {
    void* device;u32 reserved_004;
    void* source_buffers[128];void* effect_buffers[128];i32 effect_lifetimes[128];
    void* primary_buffer;u32 window;void* driver;u32 music_thread_id,music_thread,reserved_61c;
    i32 pending_effects[12],pan_count[12],pan_values[12][128];
    MusicFormat* cached_formats[16];u8* cached_data[16];u8* cached_cursors[16];u32 cached_sizes[16];
    i32 current_cache_index;MusicFormat* formats;
    MusicCommand commands[32];
    char music_names[16][256],music_filename[256];
    NotifiedSoundStream* music;u32 notification_event;
    u32 play_argument,archive_offset,device_worker,source_worker,device_worker_id;
    i32 load_stop;u32 startup_window,loading_done;
    u8* pending_waves[37];i32 music_volume,effects_volume,effects_gain;
    void queue_effect(i32 effect,i32 pan,const SoundDefinition* definitions) noexcept;
    void queue_effect_position(i32 effect,float position,const SoundDefinition* definitions) noexcept;
    void stop_effect(i32 effect) noexcept;
    void queue_music(i32 kind,i32 argument,const char* filename);
};
static_assert(offsetof(AudioManager,pending_effects)==0x620&&offsetof(AudioManager,pan_values)==0x680);
static_assert(offsetof(AudioManager,formats)==0x1f84&&offsetof(AudioManager,commands)==0x1f88);
static_assert(offsetof(AudioManager,music)==0x5208&&sizeof(AudioManager)==0x52d0);
}
