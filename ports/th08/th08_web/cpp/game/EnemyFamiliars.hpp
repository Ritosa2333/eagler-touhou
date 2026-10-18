#pragma once
#include "EnemyDrops.hpp"
#include "GameGauge.hpp"
#include "DamageRegions.hpp"
namespace th08 {
struct EnemyFamiliarContext {
    Timer form_transition,gauge_idle,item_gauge_lock;
    u8 character=0,bomb=0,boss_present=0,time_spell=0;
};
struct EnemyFamiliarActions:EnemyDropActions {
    virtual void popup(const Vec3& position,i32 number,i32 multiplier,u32 color)=0;
    virtual void popup_scale(float x,float y)=0;
    virtual void panned_sound(i32 index,float x)=0;
};
void unlink_familiar(EclVm&)noexcept;
// Original 0042adb0: parent-wide clearing and an individually defeated familiar
// have different rewards, gauge timers and cancellation regions.
class EnemyFamiliars {
    EnemyFamiliarContext& input;GameGauge& gauge;Rng& random;EnemyDropSequence& drops;DamageRegions& regions;EnemyFamiliarActions& actions;
    void scatter(const Vec3&,float radius,i32 mode);
public:
    EnemyFamiliars(EnemyFamiliarContext& i,GameGauge& g,Rng& r,EnemyDropSequence& d,DamageRegions& p,EnemyFamiliarActions& a):input(i),gauge(g),random(r),drops(d),regions(p),actions(a){}
    bool clear(EclVm&,bool reward);
};
}
