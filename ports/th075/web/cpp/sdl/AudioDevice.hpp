#pragma once
#include "../game/GameResources.hpp"
#include <memory>
namespace th075::sdl {
class AudioDevice {
 struct Impl;std::unique_ptr<Impl> impl;
public:
 bool musicEnabled=true;void close();const u32* statistics();
 std::string error;i32 musicVolume=100,soundVolume=100;
 AudioDevice();~AudioDevice();bool initialize(ResourceStore&);
 bool character_bank(u8 side,const SoundPack&);void release_character(u8 side);
 void sound(i16 id);void character_sound(u8 side,i16 id);
 bool select_music(i16 cue);void start_music();void stop_music();
 void fade_music(i32 target,i32 step,u8 stopWhenDone,u8 startWhenDone);void music_level(i32);
 void update(u32 now);void pump();void suspend(bool);bool mix(float*,u32 frames);
};
}
