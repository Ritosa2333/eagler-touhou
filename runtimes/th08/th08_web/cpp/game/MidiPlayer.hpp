// MIDI sequencer recovered from TH08 1.00d and the MIT reference Midi.cpp.
#pragma once
#include "Arithmetic.hpp"
#include <array>
#include <vector>
namespace th08 {
struct MidiChannel {u8 keys[16]{},instrument=0,bank=0,pan=0,reverb=0,chorus=0,volume=0,modified_volume=0;};
static_assert(sizeof(MidiChannel)==23);
struct MidiTrackState {i32 playing=0,next=0;u32 length=0;u8 opcode=0,padding[3]{};u8* data=nullptr;u8* cursor=nullptr;u8* loop=nullptr;u32 loop_tick=0;};
static_assert(sizeof(MidiTrackState)==32);
struct MidiDeviceSink {
    virtual ~MidiDeviceSink()=default;
    virtual void open()=0;virtual void close()=0;
    virtual void short_message(u8 status,u8 a,u8 b)=0;
    virtual void long_message(const u8*,u32)=0;
};
class MidiPlayer {
    MidiDeviceSink& device;std::array<std::vector<u8>,32> files;std::vector<std::vector<u8>> track_data;
    bool failed=false,playing=false;u32 variable(MidiTrackState&);bool available(const MidiTrackState&,u32)const;
    u8 next(MidiTrackState&);u64 current_tick()const;
public:
    i32 file_index=-1,header_cursor=0;u32 format=0;i32 divisions=0,tempo=0;
    u64 elapsed=0,base_ticks=0;MidiChannel channels[16]{};i8 transpose=0;
    float fade_multiplier=0;u32 fade_last=0;bool suppress_fade=false; i32 fading=0,fade_interval=0,fade_elapsed=0;
    i32 loop_tempo=0;u64 loop_elapsed=0,loop_base=0;std::vector<MidiTrackState> tracks;
    explicit MidiPlayer(MidiDeviceSink& d):device(d){}
    bool load(i32,const u8*,u32);void release(i32);void clear_tracks();bool parse(i32);bool load_file(const u8*,u32);
    bool play();bool stop();void load_tracks();void fade(u32);void fade_volume(i32);
    bool tick();bool process(MidiTrackState&);bool active()const{return playing;}bool invalid()const{return failed;}
};
}
