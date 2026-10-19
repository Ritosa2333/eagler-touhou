#pragma once
#include "EffectState.hpp"
namespace th08 {
class EffectTransforms {
    Rng& rng;const FrameTiming& timing;
    i32 spark(EffectState&,bool large);
public:
    EffectTransforms(Rng& r,const FrameTiming& t):rng(r),timing(t){}
    i32 small_spark(EffectState& e){return spark(e,false);}
    i32 large_spark(EffectState& e){return spark(e,true);}
    i32 inward(EffectState&);
    i32 outward(EffectState&);
    static i32 accelerate(EffectState&);
    static i32 orbit(EffectState&);
    static i32 inward60(EffectState&);
    static i32 inward240(EffectState&);
    static i32 outward90(EffectState&);
    static i32 follow(EffectState&,const Vec3& player);
    static i32 edge(EffectState&);
    static i32 ring(EffectState&);
    static i32 ring_detailed(EffectState&);
    static i32 ring_timed(EffectState&);
    static i32 ring_alpha(EffectState&);
    static i32 moon(EffectState&);
};
}
