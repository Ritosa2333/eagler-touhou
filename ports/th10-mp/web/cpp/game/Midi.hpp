#pragma once
#include "Types.hpp"
namespace th10 {
struct MidiEnvironment;
struct MidiHeader {u8* data;u32 length,recorded,user,flags;MidiHeader* next;u32 reserved,offset,reserved_words[8];};
struct MidiTrack {i32 active,next_tick;u32 length;u8 running_status,reserved[3];u8 *data,*cursor,*loop_cursor;i32 loop_tick;};
struct MidiChannel {u8 notes[16],instrument,bank,pan,reverb,chorus,volume,adjusted_volume;};
struct MidiManager {
    u32 original_virtual_table,timer,period,max_period;i32 current_file;
    MidiHeader* pending[32];i32 pending_cursor;u8* files[32];
    i32 track_count,format,division,tempo;u64 ticks,tick_offset;MidiTrack* tracks;
    u32 device;i32 device_id;u8 reserved_144[16];MidiChannel channels[16];
    std::int8_t transpose;u8 reserved_2c5[3];float gain;i32 last_gain,volume,volume_mode,reserved_2d8,paused,fading,fade_duration,fade_elapsed,loop_tempo;u64 loop_ticks,loop_offset;
    void initialize(MidiEnvironment& environment);
    void release(MidiEnvironment& environment);
    i32 load_file(i32 index,const char* name,MidiEnvironment& environment);
    void unload_file(i32 index,MidiEnvironment& environment);
    void release_tracks(MidiEnvironment& environment);
    i32 parse(i32 index,MidiEnvironment& environment);
    void rewind();
    i32 start(MidiEnvironment& environment);
    i32 stop(MidiEnvironment& environment);
    i32 complete(MidiHeader* header,MidiEnvironment& environment);
    void fade(i32 duration);
    void event(MidiTrack& track,MidiEnvironment& environment);
    void tick(MidiEnvironment& environment);
    void apply_volume(i32 adjustment,MidiEnvironment& environment);
};
struct MidiEnvironment {
    u32* window;u32 timer_callback;
    virtual void timer_capabilities(u32* periods)=0;
    virtual void kill_timer(u32 timer)=0;
    virtual void end_period(u32 period)=0;
    virtual void begin_period(u32 period)=0;
    virtual u32 create_timer(u32 period,u32 callback,void* owner)=0;
    virtual void reset_device(u32 handle)=0;
    virtual void close_device(u32 handle)=0;
    virtual void open_device(u32* handle,i32 device,u32 window)=0;
    virtual void unprepare(u32 handle,MidiHeader& header)=0;
    virtual i32 prepare(u32 handle,MidiHeader& header)=0;
    virtual i32 send_long(u32 handle,MidiHeader& header)=0;
    virtual void send_short(u32 handle,u32 message)=0;
    virtual u8* allocate(u32 size)=0;
    virtual void release_bytes(void* bytes)=0;
    virtual u8* read_file(const char* name)=0;
    virtual void missing_file(const char* name)=0;
};
u32 midi_variable(u8*& cursor);
static_assert(sizeof(MidiHeader)==64&&sizeof(MidiTrack)==32&&sizeof(MidiChannel)==23);
static_assert(offsetof(MidiManager,pending)==0x14&&offsetof(MidiManager,tracks)==0x138&&offsetof(MidiManager,channels)==0x154&&offsetof(MidiManager,gain)==0x2c8&&sizeof(MidiManager)==0x300);
}
