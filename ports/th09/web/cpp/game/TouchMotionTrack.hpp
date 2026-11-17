#pragma once
#include <array>
#include <vector>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <algorithm>
#include "ReplayCompression.hpp"

namespace th09::touch {
// Adapted from portable/input/MotionTrack.hpp. TH09 has 10 stages x 2 sides.
// A sparse stream sampled at the player's movement callback, after loading.
// Each entry overrides one movement with its pre-timescale velocity. Ordinary
// keyboard frames and the original replay byte stream remain unchanged.
class MotionTrack {
public:
    struct Entry {std::uint32_t tick;float x,y;};
    static constexpr std::uint32_t limit=600000;
    static constexpr float velocity_limit=10000000;
    std::array<std::vector<Entry>,20> stages;
    std::array<std::uint32_t,20> ticks{},cursors{};
    bool playing=false,recording=false,invalid=false,active=false,unlimited=false;
    float target_x=0,target_y=0;
    void clear(){for(auto& s:stages)s.clear();ticks={};cursors={};playing=recording=invalid=active=unlimited=false;target_x=target_y=0;}
    void begin(int stage,bool initial,bool replay,bool record){
        if(initial&&!replay)clear();playing=replay;recording=record;active=false;
        if(stage>=0&&stage<20){ticks[stage]=cursors[stage]=0;if(!replay)stages[stage].clear();}
    }
    bool used()const{for(const auto& s:stages)if(!s.empty())return true;return false;}
    void target(int mode,float x,float y){active=(mode==1||mode==2)&&std::isfinite(x)&&std::isfinite(y);unlimited=mode==2;if(active){target_x=x;target_y=y;}}
    bool playback(int stage,float& x,float& y){
        if(stage<0||stage>=20)return false;auto& next=cursors[stage];const auto tick=ticks[stage]++;const auto& s=stages[stage];
        while(next<s.size()&&s[next].tick<tick)next++;
        if(next==s.size()||s[next].tick!=tick)return false;x=s[next].x;y=s[next].y;next++;return true;
    }
    void record(int stage,bool enabled,float x,float y){
        if(stage<0||stage>=20)return;const auto tick=ticks[stage]++;if(!recording||!enabled)return;
        if(stages[stage].size()>=limit||!std::isfinite(x)||!std::isfinite(y)||std::abs(x)>velocity_limit||std::abs(y)>velocity_limit){invalid=true;return;}stages[stage].push_back({tick,x,y});
    }
    static std::uint32_t hash(const std::uint8_t* bytes,std::size_t size){std::uint32_t n=2166136261u;for(std::size_t i=0;i<size;i++)n=(n^bytes[i])*16777619u;return n;}
    static std::uint32_t word(const std::uint8_t* p){std::uint32_t n;std::memcpy(&n,p,4);return n;}
    bool load(const std::uint8_t* data,std::size_t size,std::uint32_t game){
        clear();if(size<24||std::memcmp(data+size-24,"THMOTION",8))return true;
        const auto* footer=data+size-24;const auto length=word(footer+16),version=word(footer+12);
        if(word(footer+8)!=game||(version!=2&&(game!=9||version<3||version>5))||length>size-24||length<6||length>80+limit*12)return false;
        const auto* payload=footer-length;if(hash(payload,length)!=word(footer+20))return false;
        const auto* end=footer;
        std::vector<std::uint8_t> decoded;
        bool second=false;
        if(version>=4){
            const auto plain=word(payload);
            if(plain<80)return false;
            if(version==4&&(plain>80+limit*12||(plain-80)%12||payload[4]>1))return false;
            if(version==5&&(plain>dictionary_limit||payload[4]!=0))return false;
            decoded.resize(plain);second=payload[4]!=0;
            if(!replay_compression::inflate(payload+5,length-5,decoded.data(),plain))return false;
            payload=decoded.data();end=payload+plain;
        }else if(length<80)return false;
        const auto* cursor=payload+80;
        std::uint32_t total=0;std::array<std::vector<Entry>,20> parsed;
        for(int stage=0;stage<20;stage++){
            const auto count=word(payload+stage*4);
            const auto minimum=version==5?3:version==3?1:12;
            if(count>limit-total||count>std::size_t(end-cursor)/minimum)return false;
            total+=count;
            auto& track=parsed[stage];track.resize(count);
            if(version==2){
                if(count)std::memcpy(track.data(),cursor,count*12);
                cursor+=count*12;
            }else if(version==3){if(!decode_compact(track,cursor,end))return false;}
            else if(version==4){unpack_lanes(track,cursor,second);cursor+=count*12;}
            else if(!unpack_dictionary(track,cursor,end))return false;
            for(std::uint32_t i=0;i<count;i++)
                if((i&&track[i].tick<=track[i-1].tick)||!std::isfinite(track[i].x)||!std::isfinite(track[i].y)||
                   std::abs(track[i].x)>velocity_limit||std::abs(track[i].y)>velocity_limit)return false;
        }
        if(cursor!=end)return false;stages=std::move(parsed);return true;
    }
    std::vector<std::uint8_t> trailer(std::uint32_t game)const{
        std::uint32_t total=0;for(const auto& s:stages)total+=s.size();if(!total||invalid||total>limit)return {};
        const std::uint32_t raw_length=80+total*12;
        std::vector<std::uint8_t> bytes(80);
        for(int stage=0;stage<20;stage++){
            const auto& track=stages[stage];const std::uint32_t count=track.size();
            std::memcpy(bytes.data()+stage*4,&count,4);
            if(game==9)encode_compact(track,bytes);
        }
        std::uint32_t version=3;
        if(game!=9||bytes.size()>=raw_length){
            version=2;bytes.resize(raw_length);std::size_t offset=80;
            for(const auto& track:stages){
                if(!track.empty())std::memcpy(bytes.data()+offset,track.data(),track.size()*12);
                offset+=track.size()*12;
            }
        }
        if(game==9){
            const auto consider=[&](const std::vector<std::uint8_t>& plain,std::uint32_t format,std::uint8_t mode){
                const std::uint32_t size=plain.size();
                const auto packed=replay_compression::deflate(plain.data(),size);
                if(packed.empty()||packed.size()+5>=bytes.size())return;
                version=format;bytes.resize(packed.size()+5);
                std::memcpy(bytes.data(),&size,4);bytes[4]=mode;
                std::copy(packed.begin(),packed.end(),bytes.begin()+5);
            };
            std::vector<std::uint8_t> lanes(raw_length);
            std::copy_n(bytes.data(),80,lanes.data());
            std::vector<std::uint8_t> dictionary(lanes.begin(),lanes.begin()+80);
            for(bool second:{false,true}){
                auto* cursor=lanes.data()+80;
                for(const auto& track:stages){pack_lanes(track,cursor,second);cursor+=track.size()*12;}
                consider(lanes,4,second);
            }
            for(const auto& track:stages)pack_dictionary(track,dictionary);
            consider(dictionary,5,0);
        }
        const std::uint32_t length=bytes.size(),checksum=hash(bytes.data(),length);
        const std::uint32_t words[]{game,version,length,checksum};bytes.resize(length+24);
        std::memcpy(bytes.data()+length,"THMOTION",8);std::memcpy(bytes.data()+length+8,words,16);return bytes;
    }
private:
    static constexpr std::uint32_t dictionary_entries=127;
    static constexpr std::uint32_t dictionary_limit=80+15*limit+40*(1+5*dictionary_entries);
    static void pack_dictionary(const std::vector<Entry>& track,std::vector<std::uint8_t>& out);
    static bool unpack_dictionary(std::vector<Entry>& track,const std::uint8_t*& cursor,const std::uint8_t* end);
    // v4 stores byte lanes of first or second differences, resetting per track.
    static void pack_lanes(const std::vector<Entry>& track,std::uint8_t* out,bool second){
        const auto count=track.size();
        for(unsigned field=0;field<3;field++){
            std::uint32_t previous=0,delta=0;
            for(std::size_t i=0;i<count;i++){
                const auto& entry=track[i];
                const auto* coordinate=field==1?&entry.x:&entry.y;
                const auto value=field==0?entry.tick:word(reinterpret_cast<const std::uint8_t*>(coordinate));
                const auto diff=value-previous,residual=second?diff-delta:diff;
                for(unsigned byte=0;byte<4;byte++)out[(field*4+byte)*count+i]=std::uint8_t(residual>>(byte*8));
                previous=value;delta=diff;
            }
        }
    }
    static void unpack_lanes(std::vector<Entry>& track,const std::uint8_t* in,bool second){
        const auto count=track.size();
        for(unsigned field=0;field<3;field++){
            std::uint32_t value=0,delta=0;
            for(std::size_t i=0;i<count;i++){
                std::uint32_t residual=0;
                for(unsigned byte=0;byte<4;byte++)residual|=std::uint32_t(in[(field*4+byte)*count+i])<<(byte*8);
                delta=second?delta+residual:residual;value+=delta;
                if(field==0)track[i].tick=value;
                else std::memcpy(field==1?&track[i].x:&track[i].y,&value,4);
            }
        }
    }
    static unsigned width(std::uint32_t value){
        return value>0xffffff?4:value>0xffff?3:value>0xff?2:value?1:0;
    }
    static void append_word(std::vector<std::uint8_t>& out,std::uint32_t value,unsigned count){
        for(unsigned i=0;i<count;i++)out.push_back(std::uint8_t(value>>(i*8)));
    }
    static std::uint32_t take_word(const std::uint8_t*& cursor,unsigned count){
        std::uint32_t value=0;
        for(unsigned i=0;i<count;i++)value|=std::uint32_t(*cursor++)<<(i*8);
        return value;
    }
    // v3 packs the gap from the next tick and XORs the exact float bits against
    // the previous sample on the same track. The control byte gives their widths.
    static void encode_compact(const std::vector<Entry>& track,std::vector<std::uint8_t>& out){
        std::uint32_t next=0,x=0,y=0;
        for(const auto& entry:track){
            const auto gap=entry.tick-next;
            const auto next_x=word(reinterpret_cast<const std::uint8_t*>(&entry.x));
            const auto next_y=word(reinterpret_cast<const std::uint8_t*>(&entry.y));
            const auto dx=x^next_x,dy=y^next_y;
            const auto t=width(gap),a=width(dx),b=width(dy);
            out.push_back(std::uint8_t(a|(b<<3)|(std::min(t,3u)<<6)));
            append_word(out,gap,t==3?4:t);append_word(out,dx,a);append_word(out,dy,b);
            next=entry.tick+1;x=next_x;y=next_y;
        }
    }
    static bool decode_compact(std::vector<Entry>& track,const std::uint8_t*& cursor,const std::uint8_t* end){
        std::uint64_t next=0;std::uint32_t x=0,y=0;
        for(auto& entry:track){
            if(cursor==end)return false;
            const unsigned control=*cursor++,a=control&7,b=(control>>3)&7;
            const unsigned gap_width=control>>6,t=gap_width==3?4:gap_width;
            if(a>4||b>4||std::size_t(end-cursor)<t+a+b)return false;
            const auto tick=next+take_word(cursor,t);
            if(tick>0xffffffffu)return false;
            x^=take_word(cursor,a);y^=take_word(cursor,b);
            entry.tick=std::uint32_t(tick);std::memcpy(&entry.x,&x,4);std::memcpy(&entry.y,&y,4);
            next=tick+1;
        }
        return true;
    }
};
inline void limit_vector(float& x,float& y,float speed){const float square=x*x+y*y;if(square>speed*speed&&square>0){const float scale=speed/std::sqrt(square);x*=scale;y*=scale;}}
}
