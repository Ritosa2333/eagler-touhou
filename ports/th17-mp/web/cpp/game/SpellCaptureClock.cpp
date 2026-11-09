#include "SpellCaptureClock.hpp"
#include <cmath>
#include <algorithm>
namespace th17 {
bool SpellCaptureClock::valid(i32 value)noexcept{return value/100000-22==((value/100)%1000+934)%1000+(value%100+67)%100;}
i32 SpellCaptureClock::encode(double seconds)noexcept{
    const double remainder=std::fmod(seconds,.0167);
    double rounded=seconds-remainder;if(remainder>=.00835)rounded+=.0167;
    const double whole=std::floor(rounded);const i32 s=whole>999?999:static_cast<i32>(whole),h=static_cast<i32>((rounded-whole)*100.0);
    return ((s+22+h)*1000+(s+66)%1000)*100+(h+33)%100;
}
bool SpellCaptureClock::present(PlayerSpellStatus& spell,i32 frames,double seconds,bool replay,std::array<i32,20>& times){
    if(!std::isfinite(seconds)){error="Invalid spell presentation clock";return false;}
    if(spell.flags&1){if(!(spell.flags&0x40)){spell.flags|=0x40;began=seconds;}return true;}
    if(!(spell.flags&0x40))return true;
    if(u32(completed)>=times.size()){error="Spell capture index outside replay header";return false;}
    displayed_frames=frames;encoded_time=encode(seconds-began);paused=0;spell.flags&=~0x40u;
    if(replay){encoded_time=times[completed];if(!valid(encoded_time))encoded_time=0x6ad1584;}
    else times[completed]=encoded_time;
    ++completed;return true;
}
}
