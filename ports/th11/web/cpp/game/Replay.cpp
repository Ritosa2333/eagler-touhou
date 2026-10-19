#include "Replay.hpp"
#include "ResourceCrypt.hpp"
#include "Lzss.hpp"
#include "GameEconomy.hpp"
#include "Rng.hpp"
#include "PlayerMotion.hpp"
#include <algorithm>
#include <cmath>

namespace th11 {
namespace {
u32 dword(const u8* p){return u32(p[0])|u32(p[1])<<8|u32(p[2])<<16|u32(p[3])<<24;}
u32 word(const u8* p){return u32(p[0])|u32(p[1])<<8;}
}
void ReplayStageState::read(const u8* p){
    seed=word(p+2);score=signed_bits(dword(p+12));power=i16(word(p+16));
    points=signed_bits(dword(p+20)*100);lives=i16(word(p+24));fragments=i16(word(p+26));
    rank=signed_bits(dword(p+28));x=signed_bits(dword(p+32));y=signed_bits(dword(p+36));
    continues=signed_bits(dword(p+40));extends=signed_bits(dword(p+44));focused=signed_bits(dword(p+48));
    graze=signed_bits(dword(p+52));weapon=signed_bits(dword(p+56));
    for(u32 i=0;i<20;++i)spell_times[i]=dword(p+60+i*4);
}
void ReplayStageState::restore(GameEconomy& e,Rng& rng,bool first)const{
    rng.seed=u16(seed);rng.calls=0;e.score_units=score;e.power=std::min(power,e.max_power);
    e.point_value=points;e.lives=lives;e.rank=rank;
    // Initial playback setup 4356e0 restores these. The next-stage path
    // 436f30 deliberately retains the live fragment and graze counters.
    if(first){e.life_fragments=fragments;e.graze=graze;}
}
void ReplayStageState::restore_position(PlayerMotionState& s)const{
    s.x=x;s.y=y;s.position.x=float(double(x)*.0078125);s.position.y=float(double(y)*.0078125);
    s.focused=focused;s.weapon_mode=weapon;
}
bool Replay::open(const u8* data,u32 size){
    bytes.clear();stages={};touches={};count=selected=cursor=touch_cursor=0;input={};failure.clear();metadata_only=false;
    auto fail=[&](const char* why){failure=why;return false;};
    if(!data||size<0x24||dword(data)!=0x72313174||word(data+4)!=4)return fail("not a TH11 1.00a replay");
    const u32 packed=dword(data+0x1c),unpacked=dword(data+0x20);
    // Bounds are checked before allocations or any stage/input pointer use.
    if(!packed||packed>size-0x24||packed>64*1024*1024||unpacked<0x70||unpacked>64*1024*1024)return fail("invalid replay sizes");
    std::vector<u8> first(packed),second(packed),decoded(unpacked);
    if(!resource_crypt(data+0x24,first.data(),packed,{0xaa,0xe1,0x800,packed},false)||
       !resource_crypt(first.data(),second.data(),packed,{0x3d,0x7a,0x40,packed},false))return fail("replay decrypt failed");
    Lzss codec;u32 written=0;
    if(!codec.decode(second.data(),packed,decoded.data(),unpacked,written)||written!=unpacked)return fail("replay decompression failed");
    const u32 n=dword(decoded.data()+0x58);
    if(!n||n>7||dword(decoded.data()+0x5c)>1||dword(decoded.data()+0x60)>2||dword(decoded.data()+0x64)>4)return fail("invalid replay selection");
    u32 offset=0x70;std::array<ReplayStage,8> parsed{};
    for(u32 i=0;i<n;++i){
        if(offset>unpacked||unpacked-offset<0x90)return fail("truncated replay stage");
        const auto* p=decoded.data()+offset;const u32 st=word(p),frames=dword(p+4),payload=dword(p+8);
        if(st<1||st>7||parsed[st].number)return fail("invalid replay stage number");
        if(payload>unpacked-offset-0x90||frames>payload/6)return fail("truncated replay inputs");
        // Native reads one FPS byte per 30 ticks. Payload may contain one
        // extra byte when the final all-ffff marker was appended on save.
        const u32 fps=frames>1?1+(frames-2)/30:0;
        if(payload-frames*6<fps)return fail("truncated replay frame rates");
        parsed[st]={st,offset,frames,payload};offset+=0x90+payload;
    }
    if(offset!=unpacked)return fail("unexpected replay data after stages");
    // Optional browser continuous-motion data lives in a USER block, outside
    // the original encrypted payload. Ordinary original files need no block.
    for(u32 tail=0x24+packed;size-tail>=12;){
        if(dword(data+tail)!=0x52455355)break;
        const u32 length=dword(data+tail+4);if(length<12||length>size-tail)return fail("invalid replay USER block");
        if(dword(data+tail+8)==0x11){
            if(length<32||dword(data+tail+12)!=0x54313154||dword(data+tail+16)!=1)return fail("invalid replay touch header");
            const u32 entries=dword(data+tail+20);if(entries>(length-32)/16||entries*16!=length-32)return fail("invalid replay touch length");
            for(u32 i=0;i<entries;++i){const u8* p=data+tail+32+i*16;const u32 st=word(p),mode=word(p+2),frame=dword(p+4);float x,y;std::memcpy(&x,p+8,4);std::memcpy(&y,p+12,4);
                if(st<1||st>7||!parsed[st].number||frame>=parsed[st].frames||mode<1||mode>2||!std::isfinite(x)||!std::isfinite(y)||(!touches[st].empty()&&frame<=touches[st].back().frame))return fail("invalid replay touch entry");
                touches[st].push_back({frame,i32(mode),x,y});
            }
        }
        tail+=length;
    }
    bytes.swap(decoded);stages=parsed;count=n;return true;
}
const ReplayStage* Replay::stage(u32 number)const{return number<stages.size()&&stages[number].number?&stages[number]:nullptr;}
const u8* Replay::header(u32 number)const {auto* s=stage(number);return s?bytes.data()+s->offset:nullptr;}
u32 Replay::character()const{return bytes.empty()?0:dword(bytes.data()+0x5c);}
u32 Replay::subtype()const{return bytes.empty()?0:dword(bytes.data()+0x60);}
u32 Replay::difficulty()const{return bytes.empty()?0:dword(bytes.data()+0x64);}
bool Replay::uses_touch()const{for(const auto& stream:touches)if(!stream.empty())return true;return false;}
void Replay::retain_metadata(){
    if(metadata_only||bytes.empty())return;
    std::vector<u8> headers(bytes.begin(),bytes.begin()+0x70);
    for(auto& stage:stages)if(stage.number){const u32 offset=u32(headers.size());headers.insert(headers.end(),bytes.begin()+stage.offset,bytes.begin()+stage.offset+0x90);stage.offset=offset;}
    bytes.swap(headers);touches={};selected=cursor=touch_cursor=0;input={};metadata_only=true;
}
bool Replay::select(u32 number){if(metadata_only||!stage(number))return false;selected=number;cursor=touch_cursor=0;input={};return true;}
ReplayInput Replay::tick(bool active){
    if(!active)return input;
    input.previous=input.held;input.end_marker=false;
    input.touch_mode=0;input.touch_x=input.touch_y=0;
    const auto* s=stage(selected);
    if(s&&cursor<s->frames){
        const auto* p=bytes.data()+s->offset+0x90+cursor*6;
        input.held=word(p);input.pressed=word(p+2);input.released=word(p+4);
        input.end_marker=input.held==0xffff&&input.pressed==0xffff&&input.released==0xffff;
        // 436190 advances the FPS pointer AFTER ticks 0,30,60,... .
        const u32 fps_index=cursor?1+(cursor-1)/30:0;
        const u32 fps_size=s->payload_size-s->frames*6;
        input.fps=fps_size?bytes[s->offset+0x90+s->frames*6+(fps_index<fps_size?fps_index:fps_size-1)]:0;
    }else{input.held=input.pressed=input.released=0;}
    if(selected<touches.size()&&touch_cursor<touches[selected].size()){const auto& t=touches[selected][touch_cursor];if(t.frame==cursor){input.touch_mode=t.mode;input.touch_x=t.x;input.touch_y=t.y;++touch_cursor;}}
    ++cursor;return input;
}
}
