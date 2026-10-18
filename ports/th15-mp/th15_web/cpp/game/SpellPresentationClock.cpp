#include "SpellPresentationClock.hpp"
#include <cmath>
#include <limits>
namespace th15 {
namespace {
i32 integer(double value)noexcept{return std::isfinite(value)&&value>=-2147483648.0&&value<2147483648.0?i32(value):std::numeric_limits<i32>::min();}
}
i32 SpellPresentationClock::encode(i32 seconds,i32 hundredths)noexcept{
 const i32 checksum=wrapping_add(wrapping_add(seconds,hundredths),22);
 return wrapping_add(wrapping_mul(wrapping_add(wrapping_mul(checksum,1000),wrapping_add(seconds,66)%1000),100),wrapping_add(hundredths,33)%100);
}
bool SpellPresentationClock::valid(i32 value)noexcept{return value/100000-22==((value/100)%1000+934)%1000+(value%100+67)%100;}
SpellPresentationClock::Event SpellPresentationClock::sample(u32& flags,i32 active_frames,double now)noexcept{
 if(flags&1){if(flags&0x40)return Event::idle;origin=now;flags|=0x40;return Event::started;}
 if(!(flags&0x40))return Event::idle;
 completed_frames=active_frames;
 const double elapsed=now-origin,remainder=std::fmod(elapsed,.0167);
 double rounded=elapsed-remainder;if(remainder>=.00835)rounded+=.0167;
 const double whole=std::floor(rounded);const i32 seconds=integer(whole),hundredths=integer((rounded-whole)*100.0);
 encoded=encode(seconds>999?999:seconds,hundredths);flags&=~0x40u;
 return Event::completed;
}
}
