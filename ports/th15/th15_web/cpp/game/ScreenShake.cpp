#include "ScreenShake.hpp"
namespace th15 {
i32 ScreenNudge::update(const ScreenShakeContext& c)noexcept{
    if(c.shutdown)return 7;age.tick(&c.rate);if(age.current>=spec.duration)return 7;
    const float amplitude=float(float(float(float(wrapping_sub(spec.last,spec.first))*age.fractional)/float(spec.duration))+float(spec.first));
    for(u32 axis=0;axis<2;axis++){const u32 direction=random.next32()%3;float world=0,screen=0;if(direction){screen=float(c.resolution_scale*amplitude);world=direction==1?amplitude:-amplitude;if(axis&&direction==2)screen=-screen;}if(axis){world_offset.y=world;screen_offset.y=screen;}else{world_offset.x=world;screen_offset.x=screen;}}return 1;
}
i32 ScreenShake::update(const ScreenShakeContext& c)noexcept{
    if(c.shutdown)return 7;if(!c.game_available||(c.game_flags&0x77))return 1;age.tick(&c.rate);float strength;
    if(age.current<spec.ramp)strength=float(age.fractional/float(spec.ramp));
    else if(age.current<wrapping_add(spec.ramp,spec.hold))strength=1;
    else{const i32 end=wrapping_add(wrapping_add(spec.ramp,spec.hold),spec.fade);if(end<=age.current)return 7;strength=float(float(float(u32(end))-age.fractional)/float(u32(spec.fade)));}
    strength=float(float(spec.amplitude)*strength);for(u32 axis=0;axis<2;axis++){const u32 direction=random.next32()%3;const float scaled=float(c.resolution_scale*strength);float a=0,b=0;if(direction==1){a=strength;b=scaled;}else if(direction==2){a=-strength;b=-scaled;}if(axis){world_offset.y=a;screen_offset.y=b;}else{world_offset.x=a;screen_offset.x=b;}}return 1;
}
}
