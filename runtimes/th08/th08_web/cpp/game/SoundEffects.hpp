// TH08 sound definitions and queue rules; MIT reference attribution in licenses.
#pragma once
#include "Arithmetic.hpp"
namespace th08 {
struct SoundDefinition {i32 sample;i16 volume,metadata;};
extern const SoundDefinition sound_definitions[46];
extern const char* const sound_samples[36];
struct AudioSink {
    virtual ~AudioSink()=default;
    virtual i32 stop(u32 buffer)=0;
    virtual i32 position(u32 buffer,u32 bytes)=0;
    virtual i32 pan(u32 buffer,i32 value)=0;
    virtual i32 volume(u32 buffer,i32 value)=0;
    virtual i32 play(u32 buffer,u32 priority,u32 flags)=0;
};
struct SoundQueue {
    i32 metadata[128]{};
    i32 indices[12]{},counts[12]{},pans[12][128]{};
};
class SoundEffects {
public:
    explicit SoundEffects(AudioSink& output):output(output){reset();}
    SoundQueue state;
    u32 buffers[46]{};
    bool initialized=true,enabled=true;
    i32 master_volume=100;
    void reset();
    void enqueue(i32 index,i32 pan=0);
    void positioned(i32 index,float x);
    void process();
    static i32 adjusted_volume(i32 volume,i32 master,bool music);
private:
    AudioSink& output;
};
struct AudioFadeState {
    i32 progress=0,total=0,type=0;
    u32 priority=0,flags=0;
    bool playing=false;
};
class AudioFades {
public:
    explicit AudioFades(AudioSink& output):output(output){}
    AudioFadeState state;
    u32 buffer=0;
    i32 master_volume=100;
    i32 volume(i32 level);
    void fade(i32 type,float seconds);
    i32 update(i32 type);
    void update_all();
    i32 stop();
    i32 pause();
    i32 unpause();
    i32 play(u32 priority,u32 flags);
private:
    AudioSink& output;
};
}
