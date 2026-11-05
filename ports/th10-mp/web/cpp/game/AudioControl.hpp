#pragma once
#include "AudioManager.hpp"
namespace th10 {
struct AudioControlEnvironment {
    const u32* display_flags;
    const u8 *music_enabled,*effects_enabled;
    const i32* effects_volume;
    const SoundDefinition* definitions;
    NotifiedSoundStream** global_music;
    virtual void prepare_track(AudioManager& manager,i32 index,const char* name)=0;
    virtual i32 start_track(AudioManager& manager,i32 index)=0;
    virtual void stop_music(NotifiedSoundStream& music)=0;
    virtual i32 reset_music(NotifiedSoundStream& music)=0;
    virtual i32 fill_music(NotifiedSoundStream& music,void* buffer,bool repeat)=0;
    virtual void recreate_music(NotifiedSoundStream& music)=0;
    virtual void select_music(WaveReader& wave,const MusicFormat* format)=0;
    virtual void play_music(NotifiedSoundStream& music)=0;
    virtual void pause_music(NotifiedSoundStream& music)=0;
    virtual void resume_music(NotifiedSoundStream& music)=0;
    virtual void volume_music(NotifiedSoundStream& music,i32 adjustment)=0;
    virtual void delete_music(NotifiedSoundStream& music)=0;
    virtual void quit_thread(u32 id)=0;
    virtual u32 wait_thread(u32 handle,u32 milliseconds)=0;
    virtual void close_handle(u32 handle)=0;
    virtual void effect_stop(void* buffer)=0;
    virtual void effect_position(void* buffer,u32 offset)=0;
    virtual void effect_pan(void* buffer,i32 pan)=0;
    virtual void effect_volume(void* buffer,i32 gain)=0;
    virtual void effect_play(void* buffer)=0;
};
struct AudioControl {
    AudioManager& manager;AudioControlEnvironment& environment;
    i32 update();
    void unload_music();
    i32 select_music(const char* name);
    i32 track_index(const char* name) const;
private:
    enum class CommandResult { Hold,Advance,Remove,RemoveAndContinue };
    CommandResult execute(MusicCommand& command);
    void update_effects();
};
}
