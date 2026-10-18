#pragma once
#include "Types.hpp"
namespace th15 {
// TH11 MultiplayerPresentation: 48px proximity fade, independent of authority.
constexpr u8 multiplayer_player_opacity(bool local,bool ghost,double distance2)noexcept{
 return ghost?(local?112:88):local?255:distance2<2304?72:160;
}
struct MultiplayerGrazeVisual {
 u32 flags=0,color=0;Vec3 jitter{};
 void observe(u32 f,u32 c,const Vec3& j)noexcept{if(f&0x60000u){flags=f&0x60000u;color=c;jitter=j;}}
 void apply(u32& f,u32& c,Vec3& j)const noexcept{if(flags){f=(f&~0x60000u)|flags;c=color;j=jitter;}}
};
}
