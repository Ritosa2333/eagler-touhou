#include "Midi.hpp"
#include "Arithmetic.hpp"
namespace th10 {
namespace {
u32 big32(const u8* p){return static_cast<u32>(p[0])<<24|static_cast<u32>(p[1])<<16|static_cast<u32>(p[2])<<8|p[3];}
u32 big16(const u8* p){return static_cast<u32>(p[0])<<8|p[1];}
u64 elapsed_ticks(const MidiManager& midi){return static_cast<u64>(static_cast<i64>(midi.division))*midi.ticks*1000/static_cast<u64>(static_cast<i64>(midi.tempo));}
u8 midi_volume(i32 value){return value<0?0:value>127?127:static_cast<u8>(value);}
}
u32 midi_variable(u8*& cursor){u32 value=0;u8 byte;do{byte=*cursor++;value=(value<<7)+(byte&127);}while(byte&128);return value;}
// 0x43a750 intentionally preserves channel state and the inactive timing fields.
void MidiManager::initialize(MidiEnvironment& env){
    original_virtual_table=0x46f810;env.timer_capabilities(&period);original_virtual_table=0x46f80c;
    timer=device=0;device_id=0;tracks=nullptr;division=tempo=track_count=0;transpose=0;gain=0;last_gain=volume=volume_mode=reserved_2d8=paused=fading=0;
    for(auto& file:files)file=nullptr;for(auto& header:pending)header=nullptr;pending_cursor=0;current_file=-1;
}
i32 MidiManager::complete(MidiHeader* header,MidiEnvironment& env){
    if(!header)return -1;for(auto& pending_header:pending)if(pending_header==header){pending_header=nullptr;env.unprepare(device,*header);if(header->data){env.release_bytes(header->data);header->data=nullptr;}env.release_bytes(header);return 0;}return -1;
}
i32 MidiManager::stop(MidiEnvironment& env){
    if(!tracks)return -1;for(auto* header:pending)if(header)complete(header,env);
    if(timer)env.kill_timer(timer);env.end_period(period);timer=0;
    if(device){env.reset_device(device);env.close_device(device);device=0;}current_file=-1;return 0;
}
void MidiManager::release_tracks(MidiEnvironment& env){
    for(i32 i=0;i<track_count;i++)if(tracks[i].data){env.release_bytes(tracks[i].data);tracks[i].data=nullptr;}
    if(tracks){env.release_bytes(tracks);tracks=nullptr;}tracks=nullptr;track_count=0;
}
void MidiManager::unload_file(i32 index,MidiEnvironment& env){if(static_cast<u32>(index)>=32)return;if(files[index]){env.release_bytes(files[index]);files[index]=nullptr;}files[index]=nullptr;}
i32 MidiManager::load_file(i32 index,const char* name,MidiEnvironment& env){
    if(static_cast<u32>(index)>=32)return -1;if(current_file==index)stop(env);unload_file(index,env);files[index]=env.read_file(name);if(!files[index]){env.missing_file(name);return -1;}return 0;
}
void MidiManager::release(MidiEnvironment& env){
    original_virtual_table=0x46f80c;stop(env);release_tracks(env);for(i32 i=0;i<32;i++)unload_file(i,env);
    if(device){env.reset_device(device);env.close_device(device);device=0;}original_virtual_table=0x46f810;
    if(timer)env.kill_timer(timer);env.end_period(period);timer=0;env.end_period(period);
}
i32 MidiManager::parse(i32 index,MidiEnvironment& env){
    release_tracks(env);if(static_cast<u32>(index)>=32||!files[index])return -1;auto* bytes=files[index];const u32 header_length=big32(bytes+4);
    format=big16(bytes+8);division=big16(bytes+12);track_count=big16(bytes+10);auto* input=bytes+8+header_length;
    tracks=reinterpret_cast<MidiTrack*>(env.allocate(static_cast<u32>(track_count)*sizeof(MidiTrack)));if(!tracks&&track_count)return -1;std::memset(tracks,0,static_cast<u32>(track_count)*sizeof(MidiTrack));
    for(i32 i=0;i<track_count;i++){const u32 length=big32(input+4);tracks[i].length=length;tracks[i].data=env.allocate(length);if(!tracks[i].data&&length)return -1;tracks[i].active=1;std::memcpy(tracks[i].data,input+8,length);input+=8+length;}
    current_file=index;tempo=1000000;return 0;
}
void MidiManager::rewind(){
    gain=1;paused=fading=0;ticks=tick_offset=0;auto* track=tracks;
    for(i32 i=0;i<track_count;i++,track++){track->cursor=track->loop_cursor=track->data;track->active=1;track->next_tick=static_cast<i32>(midi_variable(track->cursor));}
}
i32 MidiManager::start(MidiEnvironment& env){
    if(!tracks)return -1;rewind();if(device&&device_id!=-1){env.reset_device(device);env.close_device(device);device=0;}
    if(!device){device_id=-1;env.open_device(&device,-1,*env.window);}
    if(timer)env.kill_timer(timer);env.end_period(period);timer=0;env.begin_period(period);timer=env.create_timer(period,env.timer_callback,this);return 0;
}
void MidiManager::fade(i32 duration){gain=0;fade_duration=duration;fade_elapsed=paused=0;fading=1;}
void MidiManager::apply_volume(i32 adjustment,MidiEnvironment& env){
    if(volume_mode)return;
    for(u32 channel=0;channel<16;channel++){
        const auto value=midi_volume(wrapping_add((Extended::from_int(channels[channel].volume)*number(gain)).truncate_int(),adjustment));
        // WinMM ignores the high byte; the original leaves the argument's high byte intact.
        if(device)env.send_short(device,(static_cast<u32>(adjustment)&0xff000000u)|(static_cast<u32>(value)<<16)|0x700|0xb0|channel);
    }
}
void MidiManager::event(MidiTrack& track,MidiEnvironment& env){
    u8 status=*track.cursor;if(status<128)status=track.running_status;else++track.cursor;
    const u8 kind=status&0xf0,channel=status&15;u8 first=0,second=0;
    u32 message_high=static_cast<u32>(reinterpret_cast<uintptr_t>(&track))&0xff000000u;
    switch(kind){
    case 0x80:case 0x90:case 0xa0:case 0xb0:case 0xe0:first=*track.cursor++;second=*track.cursor++;break;
    case 0xc0:case 0xd0:first=*track.cursor++;break;
    case 0xf0:
        if(status==0xf0){
            if(pending[pending_cursor])complete(pending[pending_cursor],env);
            auto* allocated=reinterpret_cast<MidiHeader*>(env.allocate(sizeof(MidiHeader)));pending[pending_cursor]=allocated;auto* header=pending[pending_cursor];
            const u32 length=midi_variable(track.cursor);if(!header)return;std::memset(header,0,sizeof(*header));
            header->data=env.allocate(length+1);if(!header->data){env.release_bytes(header);pending[pending_cursor]=nullptr;return;}header->data[0]=0xf0;header->flags=0;header->length=length+1;
            for(i32 i=0;i<static_cast<i32>(length);i++){header->data[i+1]=*track.cursor;++track.cursor;}
            if(device&&(env.prepare(device,*header)!=0||env.send_long(device,*header)!=0)){
                if(header->data){env.release_bytes(header->data);header->data=nullptr;}env.release_bytes(header);pending[pending_cursor]=nullptr;
            }
            pending_cursor=wrapping_add(pending_cursor,1)%32;
        }else if(status==0xff){
            const u8 type=*track.cursor++;const u32 length=midi_variable(track.cursor);
            if(type==0x2f){track.active=0;return;}
            if(type==0x51){
                tick_offset+=elapsed_ticks(*this);ticks=0;tempo=0;
                // The shipped executable multiplies by 257, including for three-byte tempos.
                for(i32 i=0;i<static_cast<i32>(length);i++){tempo=static_cast<i32>(static_cast<u32>(tempo)*257+*track.cursor);++track.cursor;}
            }else track.cursor+=length;
        }
        break;
    }
    if(kind==0x80||kind==0x90){
        first=static_cast<u8>(first+transpose);
        // Byte addressing also preserves the executable's behavior for an out-of-range transposed pitch.
        auto* notes=reinterpret_cast<u8*>(this)+offsetof(MidiManager,channels)+channel*sizeof(MidiChannel);const u8 mask=1u<<(first&7);
        if(kind==0x90&&second)notes[first>>3]|=mask;else notes[first>>3]&=static_cast<u8>(~mask);
    }else if(kind==0xc0)channels[channel].instrument=first;
    else if(kind==0xb0)switch(first){
        case 0:channels[channel].bank=second;break;
        case 2:{auto* p=tracks;for(i32 i=0;i<track_count;i++,p++){p->loop_cursor=p->cursor;p->loop_tick=p->next_tick;}loop_tempo=tempo;loop_ticks=ticks;loop_offset=tick_offset;break;}
        case 4:{auto* p=tracks;for(i32 i=0;i<track_count;i++,p++){p->cursor=p->loop_cursor;p->next_tick=p->loop_tick;}tempo=loop_tempo;ticks=loop_ticks;tick_offset=loop_offset;break;}
        case 7:channels[channel].volume=second;second=midi_volume((Extended::from_int(second)*number(gain)).truncate_int());channels[channel].adjusted_volume=second;message_high=0;break;
        case 10:channels[channel].pan=second;break;
        case 91:channels[channel].reverb=second;break;
        case 93:channels[channel].chorus=second;break;
    }
    if(status<0xf0&&device)env.send_short(device,message_high|status|(static_cast<u32>(first)<<8)|(static_cast<u32>(second)<<16));
    track.running_status=status;const u32 delta=midi_variable(track.cursor);track.next_tick=wrapping_add(track.next_tick,static_cast<i32>(delta));
}
void MidiManager::tick(MidiEnvironment& env){
    u64 clock=elapsed_ticks(*this)+tick_offset;
    if(fading){
        if(fade_elapsed>=fade_duration){gain=0;return;}
        const auto wide=number(1)-(Extended::from_int(fade_elapsed)/Extended::from_int(fade_duration));gain=wide.to_float();
        if((wide*number(128)).truncate_int()!=last_gain)apply_volume(0,env);
        last_gain=(number(gain)*number(128)).truncate_int();fade_elapsed=wrapping_add(fade_elapsed,1);
    }
    bool any_active=false;
    for(i32 i=0;i<track_count;i++)if(tracks[i].active){
        any_active=true;
        while(clock>=static_cast<u64>(static_cast<i64>(tracks[i].next_tick))){event(tracks[i],env);clock=elapsed_ticks(*this)+tick_offset;if(!tracks[i].active)break;}
    }
    ++ticks;if(!any_active)rewind();
}
}
