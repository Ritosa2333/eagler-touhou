#include "ShotFiles.hpp"
#include "ReplayRecorder.hpp"
#include "Lzss.hpp"
#include "ResourceCrypt.hpp"
#include "Timer.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
namespace th11 {
namespace {
void word(u8* p,u32 v){p[0]=u8(v);p[1]=u8(v>>8);}
void dword(u8* p,u32 v){word(p,v);word(p+2,v>>16);}
}
bool ReplayRecorder::begin(i32 character,i32 subtype,i32 difficulty,bool practice,u64 timestamp){
    header={};stages={};selected=0;valid=false;
    if(!player_selection_valid(character,subtype)||difficulty<0||difficulty>4)return false;
    header[10]=practice;dword(header.data()+12,u32(timestamp));dword(header.data()+16,u32(timestamp>>32));
#if PORT_TITLE == 14
    dword(header.data()+0x7c,u32(character));dword(header.data()+0x80,u32(subtype));dword(header.data()+0x84,u32(difficulty));
    dword(header.data()+0x90,0xffffffffu);
#else
    dword(header.data()+92,u32(character));dword(header.data()+96,u32(subtype));dword(header.data()+100,u32(difficulty));return valid=true;
#endif
    return valid=true;
}
bool ReplayRecorder::start_stage(u32 number,const ReplayStageState& s,bool initial){
    if(!valid||number<1||number>7||stages[number].present)return false;
    selected=number;auto& stage=stages[number];stage.present=true;auto* p=stage.header.data();
#if PORT_TITLE == 14
    word(p,number);word(p+2,s.seed);dword(p+0xc,u32(s.x));dword(p+0x10,u32(s.y));
    std::memcpy(p+0x14,s.th14_globals.data(),0x70);dword(p+0x84,u32(s.weapon));
    for(u32 i=0;i<20;++i)dword(p+0x88+i*4,initial?i*0xdeaddeadu:u32(0));
    dword(p+0xd8,s.th14_stage_flags);
#else
    word(p,number);word(p+2,s.seed);dword(p+12,u32(s.score));word(p+16,u32(s.power));
    const i32 points=s.points/100;dword(p+20,u32(points-points%10));word(p+24,u32(s.lives));word(p+26,u32(s.fragments));
    dword(p+28,u32(s.rank));dword(p+32,u32(s.x));dword(p+36,u32(s.y));dword(p+40,u32(s.continues));dword(p+44,u32(s.extends));
    dword(p+48,u32(s.focused));dword(p+52,u32(s.graze));dword(p+56,u32(s.weapon));
    // Native initial header seeds unused spell-time slots with this sequence.
    for(u32 i=0;i<20;++i)dword(p+60+i*4,initial?i*0xdeaddeadu:u32(0));
    dword(p+140,initial?1:0);
#endif
    return true;
}
bool ReplayRecorder::uses_touch()const{for(const auto& s:stages)if(!s.touches.empty())return true;return false;}
bool ReplayRecorder::tick(u32 held,u32 pressed,u32 released,float fps,i32 touch_mode,float x,float y){
    if(!valid||!selected||!std::isfinite(fps)||touch_mode<0||touch_mode>2||(touch_mode&&(!std::isfinite(x)||!std::isfinite(y))))return false;auto& s=stages[selected];
    if(s.terminal||s.frames>=60*60*24)return false;
    if(touch_mode)s.touches.push_back({s.frames,touch_mode,x,y});
    if(s.frames%30==0){const float rounded=fps+.5f;s.fps.push_back(u8(rounded>=256?255:truncate_int(rounded)));}
    const auto offset=s.inputs.size();s.inputs.resize(offset+6);auto* p=s.inputs.data()+offset;word(p,held);word(p+2,pressed);word(p+4,released);++s.frames;return true;
}
void ReplayRecorder::spell_times(const std::array<u32,20>& times){if(selected)for(u32 i=0;i<20;++i)dword(stages[selected].header.data()+(kPortTitle==14?0x88:60)+i*4,times[i]);}
bool ReplayRecorder::finish_stage(bool terminal){
    if(!selected)return false;auto& s=stages[selected];if(terminal&&!s.terminal){const auto offset=s.inputs.size();s.inputs.resize(offset+6,255);++s.frames;s.terminal=true;}
    return true;
}
bool ReplayRecorder::save(const char* name,i32 score,i32 reached,i32 continues,float slowdown,std::vector<u8>& out)const{
    if(!valid||!selected||!name||!std::isfinite(slowdown))return false;
    auto h=header;std::fill(h.begin(),h.begin()+8,u8(' '));for(u32 i=0;i<8&&name[i];++i)h[i]=u8(name[i]);h[8]=0;
    dword(h.data()+20,u32(score));
#if PORT_TITLE == 14
    std::memcpy(h.data()+0x74,&slowdown,4);dword(h.data()+0x88,u32(reached));dword(h.data()+0x8c,u32(continues));
#else
    std::memcpy(h.data()+84,&slowdown,4);dword(h.data()+104,u32(reached));dword(h.data()+108,u32(continues));
#endif
    u32 count=0;for(const auto& s:stages)count+=s.present;dword(h.data()+(kPortTitle==14?0x78:88),count);if(!count)return false;
    std::vector<u8> decoded(h.begin(),h.end());
    for(const auto& s:stages)if(s.present){auto entry=s.header;dword(entry.data()+4,s.frames);dword(entry.data()+8,u32(s.inputs.size()+s.fps.size()));decoded.insert(decoded.end(),entry.begin(),entry.end());decoded.insert(decoded.end(),s.inputs.begin(),s.inputs.end());decoded.insert(decoded.end(),s.fps.begin(),s.fps.end());}
    // TH14 45583f..45587c uses 7d/3a/100 then 5c/e1/400,
    // and 456450 writes t13r/version 2. Retain TH12's own pair in its build.
    Lzss codec;auto packed=codec.encode(decoded.data(),u32(decoded.size()));if(packed.empty())return false;
    const u32 n=u32(packed.size());std::vector<u8> first(n),second(n);
    if(!resource_crypt(packed.data(),first.data(),n,{0x7d,0x3a,kPortTitle==14?0x100u:0x40u,n},true)||!resource_crypt(first.data(),second.data(),n,{kPortTitle==14?u8(0x5c):u8(0x5e),0xe1,kPortTitle==14?0x400u:0x800u,n},true))return false;
    out.assign(0x24+n,0);dword(out.data(),kPortTitle==14?0x72333174u:0x72323174u);word(out.data()+4,kPortTitle==14?2:4);dword(out.data()+12,0x24+n);dword(out.data()+16,0x100);dword(out.data()+28,n);dword(out.data()+32,u32(decoded.size()));std::copy(second.begin(),second.end(),out.begin()+0x24);
    u32 touch_count=0;for(const auto& s:stages)touch_count+=u32(s.touches.size());
    if(touch_count){const u32 start=u32(out.size()),length=32+touch_count*16;out.resize(start+length);auto* p=out.data()+start;dword(p,0x52455355);dword(p+4,length);dword(p+8,0x11);dword(p+12,0x54313154);dword(p+16,1);dword(p+20,touch_count);p+=32;
        for(u32 stage=1;stage<8;++stage)for(const auto& t:stages[stage].touches){word(p,stage);word(p+2,u32(t.mode));dword(p+4,t.frame);std::memcpy(p+8,&t.x,4);std::memcpy(p+12,&t.y,4);p+=16;}
    }
    return true;
}
}
