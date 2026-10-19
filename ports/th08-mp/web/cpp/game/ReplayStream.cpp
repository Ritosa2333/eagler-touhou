#include "ReplayStream.hpp"
namespace th08 {
namespace {
void put(std::vector<u8>& bytes,u32 offset,u16 value){if(bytes.size()<offset+2)bytes.resize(offset+2);bytes[offset]=value;bytes[offset+1]=value>>8;}
}
void ReplayStream::begin_record(u16 seed){frame=transition_frames=0;input_cursor=timing_cursor=input_end=timing_end=0;stride=2;input.assign(6,0);timing.clear();put(input,4,seed);}
bool ReplayStream::begin_playback(const u8* keys,u32 key_size,const u8* times,u32 timing_size,bool debug_records){
    if(key_size<2||timing_size<2)return false;
    frame=transition_frames=0;input_cursor=timing_cursor=0;input_end=key_size;timing_end=timing_size;stride=debug_records?6:2;
    input.assign(keys,keys+key_size);timing.assign(times,times+timing_size);return true;
}
void ReplayStream::record(ReplayInputState& state){
    if(!(state.flags&4))return;
    state.previous=state.current;state.current=state.physical;
    if(state.slow_mode||state.speedhack)return;
    if(state.flags&512){if(transition_frames>2)return;transition_frames=wrapping_add(transition_frames,1);}
    input_cursor+=2;put(input,input_cursor,state.physical);input_end=input_cursor+2;
    if(frame%30==0){
        if(timing.size()<timing_cursor+2)timing.resize(timing_cursor+2);
        timing[timing_cursor]=state.timing_level|(state.timing_forced?0x80:0);
        timing[timing_cursor+1]=state.timing_level;timing_end=timing_cursor+2;++timing_cursor;
    }
    frame=wrapping_add(frame,1);
}
bool ReplayStream::play(ReplayInputState& state){
    if(!(state.flags&4)||state.slow_mode||(state.flags&512))return true;
    if(input_cursor>input.size()||input.size()-input_cursor<2)return false;
    if(frame%30==0&&(timing_cursor>=timing.size()||timing.size()-timing_cursor<2))return false;
    state.previous=state.current;state.current=u16(input[input_cursor]|(u16(input[input_cursor+1])<<8));input_cursor+=stride;
    state.repeat=0;
    if(state.previous==state.current){
        if(state.held_frames>29){state.repeat=(state.held_frames&7)==0;if(state.held_frames>37)state.held_frames=30;}
        state.held_frames=wrapping_add(state.held_frames,1);
    }else state.held_frames=0;
    if(frame%30==0){const u8 value=timing[timing_cursor+1];state.timing_level=value&0x7f;state.timing_forced=(value&128)?-1:0;++timing_cursor;}
    frame=wrapping_add(frame,1);return true;
}
void ReplayStream::stop_recording(){input_cursor+=2;put(input,input_cursor,0);input_end=input_cursor+6;if(input.size()<input_end)input.resize(input_end);}
}
