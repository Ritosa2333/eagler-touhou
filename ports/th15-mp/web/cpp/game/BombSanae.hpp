#pragma once
#include "BombController.hpp"
#include "DamageSources.hpp"
namespace th15 {
class BombSanae final:public BombController {
    EffectManager& effects;Rng& random;Rng& visual_random;DamageSources& damage;
    bool start()override;bool frame(bool&)override;bool create(u32&,i32);bool interrupt(u32,i32);
public:
    u32 field=0,aura=0;
    BombSanae(BombContext& c,EffectManager& e,Rng& r,Rng& v,DamageSources& d):BombController(c),effects(e),random(r),visual_random(v),damage(d){}
};
}
