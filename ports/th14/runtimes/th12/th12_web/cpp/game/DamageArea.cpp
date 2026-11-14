#include "DamageArea.hpp"
#include <cmath>
namespace th11 {
void DamageArea::update() noexcept {
    if(!(flags&1))return;
    movement.update_velocity();movement.update();
    radius=float(double(radius)+growth);angle=float(double(angle)+angular_velocity);
    timer.advance(-1);if(timer.current<=0)flags&=~1u;
}
bool DamageArea::overlaps(const Vec3& p,const Vec2& s)const noexcept {
    const auto q=movement.position;
    if(flags&2){const double dx=double(q.x)-p.x,dy=double(q.y)-p.y;return float(dx*dx+dy*dy)<=double(radius)*radius;}
    if(angle==0){
        const float left=float(double(p.x)-double(s.x)*.5),right=float(double(p.x)+double(s.x)*.5);
        const float top=float(double(p.y)-double(s.y)*.5),bottom=float(double(p.y)+double(s.y)*.5);
        return double(q.x)-double(size.x)*.5<=right&&left<=double(q.x)+double(size.x)*.5&&double(q.y)-double(size.y)*.5<=bottom&&top<=double(q.y)+double(size.y)*.5;
    }
    const Vec2 delta{float(double(p.x)-q.x),float(double(p.y)-q.y)};
    const auto local=rotate(delta,-angle);
    return -double(size.x)*.5<=double(local.x)+double(s.x)*.5&&double(local.x)-double(s.x)*.5<=double(size.x)*.5&&-double(size.y)*.5<=double(local.y)+double(s.y)*.5&&double(local.y)-double(s.y)*.5<=double(size.y)*.5;
}
DamageArea* DamageAreas::circle(const Vec3& p,float radius,float growth,i32 lifetime,i32 damage,const float* rate) noexcept {
    for(auto& a:areas)if(!(a.flags&1)){
        a={};a.flags=3;a.movement.position=p;a.radius=radius;a.growth=growth;
        a.timer.set(lifetime,rate);a.damage=damage;a.max_damage=999999;a.period=4;return &a;
    }return nullptr;
}
void DamageAreas::update() noexcept {for(auto& a:areas)a.update();}
bool DamageAreas::collect(const Vec3& p,const Vec2& s,i32& damage) noexcept {
    for(auto& a:areas)if(a.flags&1){
        // This gate is intentionally opposite to the laser-shot gate in TH11.
        if(a.timer.current!=a.timer.previous){if(!a.period)return false;if(a.timer.current%a.period==0)continue;}
        if(!a.overlaps(p,s))continue;
        damage=wrapping_add(damage,a.damage);a.total_damage=wrapping_add(a.total_damage,a.damage);
        if(a.total_damage>=a.max_damage)a.damage=0;
    }return true;
}
}
