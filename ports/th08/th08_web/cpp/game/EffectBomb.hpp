#pragma once
#include "EffectState.hpp"
#include "ScreenEffects.hpp"
#include "DamageRegions.hpp"
namespace th08 {
class EffectBomb {
    ScreenEffects& screen;DamageRegions& damage;
public:
    EffectBomb(ScreenEffects& s,DamageRegions& d):screen(s),damage(d){}
    static i32 pulsing(EffectState&);
    static i32 expanding(EffectState&);
    static i32 ripple(EffectState&,u32 variant);
    static i32 quartic(EffectState&);
    i32 burst(EffectState&,bool rotating);
};
}
