#include "SpellTiming.hpp"
#include "PortTitle.hpp"
#include "Timer.hpp"
#include <algorithm>
#include <cmath>
namespace th11 {
SpellTimeParts decode_spell_time(i32 value)noexcept{
#if PORT_TITLE == 14
    // Retail42e980 decodes three decimal seconds digits, including100..999.
    const i32 seconds=((value/100)%1000+934)%1000,hundredths=(value%100+67)%100;
#else
    const i32 seconds=((value/100)%100+34)%100,hundredths=(value%100+67)%100;
#endif
    if(value/100000-22!=seconds+hundredths)return {};
    return {seconds,hundredths,true};
}
bool SpellTiming::update(u32& flags,i32 frames,double now,bool replay)noexcept{
    if(flags&1){if(!(flags&0x40)){start=now;flags|=0x40;}return true;}
    if(!(flags&0x40))return true;
    if(index>=records.size()||!std::isfinite(now)||!std::isfinite(start))return false;
    last_frames=frames;const double elapsed=now-start,remainder=std::fmod(elapsed,.0167);
    double rounded=elapsed-remainder;
#if PORT_TITLE == 14
    if(remainder>=.00835)rounded+=.0167;
#else
    if(remainder>.00835)rounded+=.0167;
#endif
    const double floor=std::floor(rounded);const i32 seconds=std::min(truncate_int(floor),999),hundredths=truncate_int((rounded-floor)*100);
#if PORT_TITLE == 14
    // 41c8ea..41c928 wraps the encoded seconds after adding66, before
    // combining the three-digit field with its checksum and hundredths.
    encoded=signed_bits((u32(wrapping_add(wrapping_add(seconds,22),hundredths))*1000u+u32(wrapping_add(seconds,66)%1000))*100u+u32(wrapping_add(hundredths,33)%100));
#else
    encoded=signed_bits(u32(wrapping_add(hundredths,seconds))*100000+u32(seconds)*100+2206600+u32(wrapping_add(hundredths,33)%100));
#endif
    flags&=~0x40u;
    if(replay){encoded=signed_bits(records[index]);if(!decode_spell_time(encoded).valid)encoded=kPortTitle==14?0x6ad1584:0x6ae9c24;}
    else records[index]=u32(encoded);
    ++index;return true;
}
}
