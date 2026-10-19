#pragma once
#include "Types.hpp"
namespace th09 {
struct SoundDefinition {i32 sample;i16 volume,metadata;};
extern const SoundDefinition sound_definitions[54];
extern const char* const sound_samples[39];
struct SoundOutput {
    virtual ~SoundOutput()=default;
    virtual void sound_stop(u32)=0;
    virtual void sound_position(u32,u32)=0;
    virtual void sound_pan(u32,i32)=0;
    virtual void sound_volume(u32,i32)=0;
    virtual void sound_play(u32)=0;
};
struct SoundQueue {i32 metadata[128]{},indices[12]{},counts[12]{},pans[12][128]{};};
class SoundEffects {
    SoundOutput& output;
public:
    explicit SoundEffects(SoundOutput& o):output(o){reset();}
    SoundQueue state;bool initialized=true,enabled=true;u32 buffers[54]{};i32 master_volume=100;
    void reset();void enqueue(i32 id,i32 pan=0);void positioned(i32 id,float x);void process();
    static i32 adjusted_volume(i32 volume,i32 master,bool music=false);
};
}
