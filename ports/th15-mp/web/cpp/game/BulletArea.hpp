#pragma once
#include "BulletState.hpp"
#include <algorithm>
#include <cmath>
namespace th15 {
// A conservative union of live player geometry. Possible contacts still use
// the exact current hit/graze predicate and its damage callbacks.
struct BulletContactBounds {
    u32 active=0;bool safe=true;u8 last=3;
    Vec2 circle_min{},circle_max{},box_min{},box_max{};float circle_radius=0;
    struct Region {Vec2 position{},minimum{},maximum{};float radius=0;};std::array<Region,3> regions{};
    bool refine_circle(const Vec3&,float radius,float graze)const noexcept;
    bool refine_rectangle(const Vec3&,float x,float y)const noexcept;
    void include(unsigned seat,const PlayerCollision& player)noexcept{
        const bool first=!active;active|=1u<<seat;
        const float radius=player.enlarged?float(float(player.size_multiplier*3.6f)*player.radius):player.radius;
        const auto p=player.position,lo=player.hitbox_min,hi=player.hitbox_max;
        if(!std::isfinite(float(float(p.x+p.y)+radius))||!std::isfinite(float(float(lo.x+lo.y)+float(hi.x+hi.y))))safe=false;
        const Vec2 minimum{std::min(lo.x,hi.x),std::min(lo.y,hi.y)},maximum{std::max(lo.x,hi.x),std::max(lo.y,hi.y)};
        regions[seat]={p,minimum,maximum,std::abs(radius)};last=u8(first?seat:std::max<unsigned>(last,seat));
        if(first){circle_min=circle_max=p;box_min=minimum;box_max=maximum;}
        else{circle_min={std::min(circle_min.x,p.x),std::min(circle_min.y,p.y)};circle_max={std::max(circle_max.x,p.x),std::max(circle_max.y,p.y)};box_min={std::min(box_min.x,minimum.x),std::min(box_min.y,minimum.y)};box_max={std::max(box_max.x,maximum.x),std::max(box_max.y,maximum.y)};}
        circle_radius=std::max(circle_radius,std::abs(radius));
    }
    bool may_contact(const BulletMotion& bullet)const noexcept{
        if(!active||!(bullet.flags&2))return false;if(!safe)return true;
        Vec2 size=bullet.hitbox;if(bullet.flags&64){size.x=float(size.x*bullet.scale);size.y=float(size.y*bullet.scale);}
        const auto& p=bullet.motion.position;if(!std::isfinite(float(float(p.x+p.y)+float(size.x+size.y))))return true;
        if(bullet.flags&16){
            const float radius=std::abs(size.x),graze=float(std::max(40.f,float(radius/2.5f))+1.f),extent=float(float(circle_radius+radius)+graze);
            const float dx=p.x<circle_min.x?float(circle_min.x-p.x):p.x>circle_max.x?float(p.x-circle_max.x):0;
            const float dy=p.y<circle_min.y?float(circle_min.y-p.y):p.y>circle_max.y?float(p.y-circle_max.y):0;
            if(!std::isfinite(float(float(dx+dy)+extent)))return true;return dx<=extent&&dy<=extent&&refine_circle(p,radius,graze);
        }
        const float x=float(std::max(24.f,float(std::abs(size.x)*.5f))+1.f),y=float(std::max(24.f,float(std::abs(size.y)*.5f))+1.f);
        return !(p.x<float(box_min.x-x)||p.x>float(box_max.x+x)||p.y<float(box_min.y-y)||p.y>float(box_max.y+y))&&refine_rectangle(p,x,y);
    }
};
bool bullet_circle_area(const BulletState&,const Vec3& center,float radius,bool honor_protection)noexcept;
bool bullet_rectangle_area(const BulletState&,const Vec3& center,const Vec2& size,float angle)noexcept;
}
