#include "MidiPlayer.hpp"
#include <algorithm>
namespace th08 {
namespace {u16 be16(const u8* p){return u16(p[0])*256+p[1];}u32 be32(const u8* p){return u32(p[0])*0x1000000+u32(p[1])*65536+u32(p[2])*256+p[3];}}
bool MidiPlayer::load(i32 slot,const u8* p,u32 size){if(u32(slot)>=32||!p||size>32*1024*1024)return false;if(file_index==slot)stop();files[slot].assign(p,p+size);return true;}
void MidiPlayer::release(i32 slot){if(u32(slot)<32)files[slot].clear();}
void MidiPlayer::clear_tracks(){tracks.clear();track_data.clear();}
bool MidiPlayer::parse(i32 slot){
    clear_tracks();failed=false;if(u32(slot)>=32)return false;const auto& bytes=files[slot];if(bytes.size()<14)return false;
    const auto* p=bytes.data();const u32 length=be32(p+4);if(length<6||length>bytes.size()-8)return false;
    format=be16(p+8);divisions=be16(p+12);const u32 count=be16(p+10);if(!divisions||!count||count>1024)return false;
    u32 cursor=8+length;track_data.resize(count);tracks.resize(count);
    for(u32 i=0;i<count;++i){if(bytes.size()-cursor<8){clear_tracks();return false;}const u32 n=be32(p+cursor+4);cursor+=8;if(!n||n>bytes.size()-cursor){clear_tracks();return false;}auto& data=track_data[i];data.assign(p+cursor,p+cursor+n);cursor+=n;auto& track=tracks[i];track.playing=1;track.length=n;track.data=data.data();}
    tempo=1000000;file_index=slot;return true;
}
bool MidiPlayer::load_file(const u8* p,u32 n){if(!load(31,p,n))return false;const bool result=parse(31);release(31);return result;}
bool MidiPlayer::available(const MidiTrackState& t,u32 n)const{return t.cursor&&t.cursor>=t.data&&u64(t.cursor-t.data)+n<=t.length;}
u8 MidiPlayer::next(MidiTrackState& t){if(!available(t,1)){failed=true;t.playing=0;return 0;}return *t.cursor++;}
u32 MidiPlayer::variable(MidiTrackState& t){u32 value=0;for(u32 i=0;i<5;++i){const auto b=next(t);value=value*128+(b&127);if(!(b&128))return value;}failed=true;t.playing=0;return 0;}
void MidiPlayer::load_tracks(){fade_multiplier=1;fading=0;elapsed=base_ticks=0;for(auto& t:tracks){t.cursor=t.loop=t.data;t.playing=1;t.next=signed_bits(variable(t));}}
bool MidiPlayer::play(){if(tracks.empty())return false;load_tracks();if(failed)return false;device.open();playing=true;return true;}
bool MidiPlayer::stop(){if(tracks.empty())return false;device.close();playing=false;file_index=-1;return true;}
void MidiPlayer::fade(u32 ms){fade_multiplier=0;fade_interval=signed_bits(ms);fade_elapsed=0;fading=1;}
void MidiPlayer::fade_volume(i32 adjustment){if(suppress_fade)return;for(u32 i=0;i<16;++i){const auto volume=wrapping_add((Extended::from_int(channels[i].volume)*number(fade_multiplier)).truncate_int(),adjustment);device.short_message(u8(0xb0+i),7,u8(std::clamp(volume,0,127)));}}
u64 MidiPlayer::current_tick()const{return base_ticks+(elapsed*u64(i64(divisions))*1000)/u64(i64(tempo));}
bool MidiPlayer::tick(){
    if(failed||!tempo)return false;u64 now=current_tick();
    if(fading){if(fade_elapsed>=fade_interval){fade_multiplier=0;return true;}
        const float a=Extended::from_int(fade_elapsed).to_float(),b=Extended::from_int(fade_interval).to_float();fade_multiplier=(number(1)-number(a)/number(b)).to_float();
        const u32 level=u32((number(fade_multiplier)*number(128)).truncate_int());if(level!=fade_last)fade_volume(0);fade_last=level;fade_elapsed=wrapping_add(fade_elapsed,1);
    }
    bool had_tracks=false;u32 instructions=0;
    for(auto& t:tracks)if(t.playing){had_tracks=true;while(t.playing&&u64(i64(t.next))<=now){if(++instructions>100000||!process(t))return false;if(!tempo){failed=true;return false;}now=current_tick();}}
    ++elapsed;if(!had_tracks)load_tracks();return !failed;
}
bool MidiPlayer::process(MidiTrackState& t){
    if(!available(t,1)){failed=true;return false;}u8 opcode=*t.cursor;if(opcode<0x80)opcode=t.opcode;else ++t.cursor;
    const u8 high=opcode&0xf0,channel=opcode&15;u8 a=0,b=0;
    if(high==0xf0){
        if(opcode==0xf0){const u32 n=variable(t);if(!available(t,n)){failed=true;return false;}std::vector<u8> packet(n+1);packet[0]=0xf0;std::memcpy(packet.data()+1,t.cursor,n);t.cursor+=n;device.long_message(packet.data(),packet.size());header_cursor=(header_cursor+1)%32;}
        else if(opcode==0xff){const auto meta=next(t);const u32 n=variable(t);if(meta==0x2f){t.playing=0;return !failed;}if(!available(t,n)){failed=true;return false;}
            if(meta==0x51){if(!tempo){failed=true;return false;}base_ticks=current_tick();elapsed=0;tempo=0;
                // TH08 really accumulates previous + previous*256 + byte.
                // Preserve this original timing quirk instead of correcting it.
                for(u32 i=0;i<n;++i)tempo=signed_bits(u32(tempo)*257+next(t));
            }else t.cursor+=n;
        }
    }else if(high==0xc0||high==0xd0)a=next(t);
    else if(high>=0x80&&high<=0xe0){a=next(t);b=next(t);}else{failed=true;return false;}
    auto& ch=channels[channel];
    if(high==0x90||high==0x80){a=u8(a+transpose);if(a<128){if(high==0x90&&b)ch.keys[a>>3]|=u8(1u<<(a&7));else ch.keys[a>>3]&=u8(~(1u<<(a&7)));}}
    else if(high==0xc0)ch.instrument=a;
    else if(high==0xb0)switch(a){
        case 0:ch.bank=b;break;case 7:ch.volume=b;b=ch.modified_volume=u8(std::clamp((Extended::from_int(b)*number(fade_multiplier)).truncate_int(),0,127));break;
        case 91:ch.reverb=b;break;case 93:ch.chorus=b;break;case 10:ch.pan=b;break;
        case 2:for(auto& track:tracks){track.loop=track.cursor;track.loop_tick=u32(track.next);}loop_tempo=tempo;loop_elapsed=elapsed;loop_base=base_ticks;break;
        case 4:for(auto& track:tracks){track.cursor=track.loop;track.next=signed_bits(track.loop_tick);}tempo=loop_tempo;elapsed=loop_elapsed;base_ticks=loop_base;break;
    }
    if(opcode<0xf0)device.short_message(opcode,a,b);t.opcode=opcode;t.next=wrapping_add(t.next,signed_bits(variable(t)));return !failed;
}
}
