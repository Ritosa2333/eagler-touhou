#pragma once
#include "EffectState.hpp"
#include "Camera.hpp"
namespace th08 {
struct EffectEnvironment {
    SceneCamera camera;Vec3 player,boss;ZunColor tint;
    u8 boss_present,boss_active,any_boss,dialogue,transition;
};
class EffectSpace {
    Rng& rng;const FrameTiming& timing;const EffectEnvironment& environment;
    bool advance(EffectState&);
    void track_boss(EffectState&);
public:
    EffectSpace(Rng& r,const FrameTiming& t,const EffectEnvironment& e):rng(r),timing(t),environment(e){}
    i32 ambient(EffectState&);
    i32 ambient_step(EffectState&);
    i32 glow(EffectState&,bool tall);
    i32 glow_step(EffectState&,bool tall);
    static i32 orbit_step(EffectState&);
    void projected(AnmVm&,Vec3&);
};
}
