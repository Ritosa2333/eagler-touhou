#pragma once
#include "Assets.hpp"
#include "../game/SoundEffects.hpp"
#include <memory>
namespace th09::sdl {
class AudioDevice final:public SoundOutput {
    struct Impl;std::unique_ptr<Impl> impl;
public:
    std::string error;SoundEffects effects{*this};i32 music_volume=100;bool music_enabled=true;
    AudioDevice();~AudioDevice();
    bool initialize(ResourceReader&);
    bool music(i32 track);bool music_file(const char*);void fade_music(i32 frames=240);void pause_music(bool);
    void refresh_volume();void update();void pump();void suspend(bool);
    void sound_stop(u32)override;void sound_position(u32,u32)override;
    void sound_pan(u32,i32)override;void sound_volume(u32,i32)override;void sound_play(u32)override;
    const u32* statistics()const;
};
}
