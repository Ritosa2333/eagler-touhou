#pragma once
#include <array>
#include <vector>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <algorithm>

namespace th09::touch {
// Adapted from portable/input/MotionTrack.hpp. TH09 v2: 10 stages x 2 sides.
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
        const auto* footer=data+size-24;const auto length=word(footer+16);
        if(word(footer+8)!=game||word(footer+12)!=2||length>size-24||length<80||length>80+limit*12)return false;
        const auto* payload=footer-length;if(hash(payload,length)!=word(footer+20))return false;
        std::size_t offset=80;std::uint32_t total=0;std::array<std::vector<Entry>,20> parsed;
        for(int stage=0;stage<20;stage++){
            const auto count=word(payload+stage*4);if(count>limit-total||count>(length-offset)/12)return false;total+=count;
            auto& s=parsed[stage];s.resize(count);if(count)std::memcpy(s.data(),payload+offset,count*12);offset+=count*12;
            for(std::uint32_t i=0;i<count;i++)if((i&&s[i].tick<=s[i-1].tick)||!std::isfinite(s[i].x)||!std::isfinite(s[i].y)||std::abs(s[i].x)>velocity_limit||std::abs(s[i].y)>velocity_limit)return false;
        }
        if(offset!=length)return false;stages=std::move(parsed);return true;
    }
    std::vector<std::uint8_t> trailer(std::uint32_t game)const{
        std::uint32_t total=0;for(const auto& s:stages)total+=s.size();if(!total||invalid||total>limit)return {};
        const std::uint32_t length=80+total*12;std::vector<std::uint8_t> bytes(length+24);std::size_t offset=80;
        for(int stage=0;stage<20;stage++){const std::uint32_t n=stages[stage].size();std::memcpy(bytes.data()+stage*4,&n,4);if(n)std::memcpy(bytes.data()+offset,stages[stage].data(),n*12);offset+=n*12;}
        const auto checksum=hash(bytes.data(),length);const std::uint32_t words[]{game,2,length,checksum};std::memcpy(bytes.data()+length,"THMOTION",8);std::memcpy(bytes.data()+length+8,words,16);return bytes;
    }
};
inline void limit_vector(float& x,float& y,float speed){const float square=x*x+y*y;if(square>speed*speed&&square>0){const float scale=speed/std::sqrt(square);x*=scale;y*=scale;}}
}
