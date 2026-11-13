#pragma once
#include "Types.hpp"
#include "PortTitle.hpp"
namespace th11 {
struct SoundDefinition {i32 sample;i16 volume,metadata;
#if PORT_TITLE == 14
    u32 play_flags=0;i16 queue_metadata=0;
#endif
};
inline constexpr u32 sound_voice_count=kPortTitle==14?76:56;
extern const SoundDefinition sound_definitions[sound_voice_count];
// The title's own sample list, in the order its executable's pointer array holds the names - which is also its
// archive's order. The list is per title and so is its length: th12 has 49 entries (48 distinct, se_cardget.wav
// twice), th13 has 60 (59 distinct, the same duplicated sample). It is a pointer and a count rather than an
// array so that a title with a different length is a table, not a code change.
extern const char* const* sound_samples;
extern const u32 sound_sample_count;
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
    void queue(i32 id,i32 pan);
public:
    explicit SoundEffects(SoundOutput& o):output(o){reset();}
    SoundQueue state;bool initialized=true,enabled=true;u32 buffers[sound_voice_count]{};i32 master_volume=100;
    void reset();void enqueue(i32 id,i32 pan=0);void positioned(i32 id,float x);void stop(i32 id);void pan(i32 id,i32 value);void process();
    static i32 adjusted_volume(i32 volume,i32 master,bool music=false);
};
}
