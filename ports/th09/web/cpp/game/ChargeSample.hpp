#pragma once
#include "Timer.hpp"
#include <array>
#include <cmath>
#include <cstring>
namespace th09 {
// A locally accepted result is immutable across network correction and Replay.
struct ChargeSample {
    enum : u32 {Begin=1,End=2,LevelSound=4};
    float charge=0;
    Timer charging_time{0,0,0},full_charge{0,0,0};
    float released=-1;
    u32 events=0,active=0;
    bool valid()const{
        const auto timer=[](const Timer& t){return std::isfinite(t.time)&&t.time>=-1&&t.time<=100000000&&t.current>=-1&&t.current<=100000000&&t.previous>=-999999&&t.previous<=100000000;};
        return active<=1&&(events&~7u)==0&&std::isfinite(charge)&&charge>=0&&charge<=400&&
            std::isfinite(released)&&(released==-1||(released>=0&&released<=400))&&
            timer(charging_time)&&timer(full_charge)&&((events&End)!=0)==(released>=0);
    }
};
inline bool operator==(const ChargeSample& a,const ChargeSample& b){
    const auto timer=[](const Timer& x,const Timer& y){return x.previous==y.previous&&x.time==y.time&&x.current==y.current;};
    return a.active==b.active&&a.charge==b.charge&&a.released==b.released&&a.events==b.events&&
        timer(a.charging_time,b.charging_time)&&timer(a.full_charge,b.full_charge);
}
inline bool operator!=(const ChargeSample& a,const ChargeSample& b){return !(a==b);}
namespace charge_record {
inline constexpr u32 Bytes=40;
inline u32 word(const u8* bytes){return u32(bytes[0])|(u32(bytes[1])<<8)|(u32(bytes[2])<<16)|(u32(bytes[3])<<24);}
inline void word(u8* bytes,u32 value){for(u32 n=0;n<4;++n)bytes[n]=u8(value>>(8*n));}
inline float real(const u8* bytes){const u32 bits=word(bytes);float value;std::memcpy(&value,&bits,4);return value;}
inline void real(u8* bytes,float value){u32 bits;std::memcpy(&bits,&value,4);word(bytes,bits);}
inline void encode(const ChargeSample& sample,u8* bytes){
    real(bytes,sample.charge);const Timer* timers[]{&sample.charging_time,&sample.full_charge};
    for(u32 i=0;i<2;++i){auto* out=bytes+4+i*12;word(out,u32(timers[i]->previous));real(out+4,timers[i]->time);word(out+8,u32(timers[i]->current));}
    real(bytes+28,sample.released);word(bytes+32,sample.events);word(bytes+36,sample.active);
}
inline bool decode(const u8* bytes,ChargeSample& sample){
    sample.charge=real(bytes);Timer* timers[]{&sample.charging_time,&sample.full_charge};
    for(u32 i=0;i<2;++i){const auto* in=bytes+4+i*12;timers[i]->previous=i32(word(in));timers[i]->time=real(in+4);timers[i]->current=i32(word(in+8));}
    sample.released=real(bytes+28);sample.events=word(bytes+32);sample.active=word(bytes+36);return sample.valid();
}
}
struct PendingChargeRelease {
    std::array<float,8> values{};u32 count=0,invalid=0;
    bool push(float value){if(count==values.size()){invalid=1;return false;}values[count++]=value;return true;}
    float take(){const auto value=values[0];for(u32 i=1;i<count;++i)values[i-1]=values[i];values[--count]=0;return value;}
};
}
