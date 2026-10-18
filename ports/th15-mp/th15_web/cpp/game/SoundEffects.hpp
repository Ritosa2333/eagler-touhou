#pragma once
#include "Timer.hpp"
#include <array>
#include <string>
namespace th15 {
struct SoundDefinition {i32 id=0,sample=0;i16 volume=0,renew=0;u32 flags=0,reserved=0;};
extern const std::array<SoundDefinition,77> sound_definitions;
extern const std::array<const char*,66> sound_samples;
struct SoundOutput {
 virtual ~SoundOutput()=default;virtual bool sound_available(u32)const=0;virtual bool sound_playing(u32)=0;
 virtual void sound_stop(u32)=0;virtual void sound_position(u32,u32)=0;virtual void sound_pan(u32,i32)=0;virtual void sound_volume(u32,i32)=0;virtual void sound_play(u32,u32 flags)=0;
};
struct SoundQueue {std::array<i32,12> indices{},counts{};std::array<std::array<i32,128>,12> pans{};};
struct SoundVoice {i32 renewal=-1,pan=0;bool resume=false;const SoundDefinition* definition=nullptr;};
// Requests and mixing are separate. The original queue has 12 distinct sounds
// and 60 positions per sound; its 128-entry storage stride remains internal.
class SoundEffects {
 SoundOutput& output;void stop_voice(u32);void start_voice(u32,i32);
public:
 SoundQueue state;std::array<SoundVoice,77> voices{};bool enabled=true;i32 master_volume=100;std::string error;
 explicit SoundEffects(SoundOutput&);
 void reset();
 bool enqueue(i32 id,i32 pan=0);bool positioned(i32 id,float x);bool stop(i32 id);void process();void suspend();void resume();
 static i32 adjusted_volume(i32 volume,i32 master)noexcept;
};
}
