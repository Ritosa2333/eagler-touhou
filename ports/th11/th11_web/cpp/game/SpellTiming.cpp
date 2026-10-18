#include "SpellTiming.hpp"
#include "Timer.hpp"
#include <algorithm>
#include <cmath>
namespace th11 {
SpellTimeParts decode_spell_time(i32 value)noexcept{
    const i32 seconds=((value/100)%100+34)%100,hundredths=(value%100+67)%100;
    if(value/100000-22!=seconds+hundredths)return {};
    return {seconds,hundredths,true};
}
bool SpellTiming::update(u32& flags,i32 frames,double now,bool replay)noexcept{
    if(flags&1){if(!(flags&0x40)){start=now;flags|=0x40;}return true;}
    if(!(flags&0x40))return true;
    if(index>=records.size()||!std::isfinite(now)||!std::isfinite(start))return false;
    last_frames=frames;const double elapsed=now-start,remainder=std::fmod(elapsed,.0167);
    double rounded=elapsed-remainder;if(remainder>.00835)rounded+=.0167;
    const double floor=std::floor(rounded);const i32 seconds=std::min(truncate_int(floor),999),hundredths=truncate_int((rounded-floor)*100);
    encoded=signed_bits(u32(wrapping_add(hundredths,seconds))*100000+u32(seconds)*100+2206600+u32(wrapping_add(hundredths,33)%100));
    flags&=~0x40u;
    if(replay){encoded=signed_bits(records[index]);if(!decode_spell_time(encoded).valid)encoded=0x6ae9c24;}
    else records[index]=u32(encoded);
    ++index;return true;
}
}
