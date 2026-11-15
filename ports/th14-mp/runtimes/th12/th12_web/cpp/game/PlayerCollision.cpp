#include "PlayerCollision.hpp"
#include "PortTitle.hpp"
#include <cmath>
namespace th11 {
PlayerCollisionResult PlayerCollision::hit()const noexcept{
    if(bomb_active||state==2||state==3||state==4||(flags&2))return {};
    return {CollisionKind::Hit,invincibility_frames<=0};
}
PlayerCollisionResult PlayerCollision::rectangle(Vec2 p,Vec2 size,bool probe)const noexcept{
    if constexpr(kPortTitle==14||kPortTitle==16){
        // TH14 44ee80 / TH16 4438c0 use SSE edges and ordered JA comparisons.
        const auto intersects=[&](float hx,float hy){
            return !(minimum.x>p.x+hx||minimum.y>p.y+hy||p.x-hx>maximum.x||p.y-hy>maximum.y);
        };
        if(!intersects(size.x*.5f,size.y*.5f))return {intersects(24.f,24.f)?CollisionKind::Graze:CollisionKind::None,false};
        if(dialogue_active)return {};
        if(probe)return {CollisionKind::Graze,false};
        if(state==2||state==3||state==4)return {};
        return {CollisionKind::Hit,invincibility_frames<=0};
    }
    // The original stores the four rectangle edges as float before comparing.
    const auto intersects=[&](double half_x,double half_y){
        const float left=float(double(p.x)-half_x),right=float(double(p.x)+half_x);
        const float top=float(double(p.y)-half_y),bottom=float(double(p.y)+half_y);
        return !(right<minimum.x||bottom<minimum.y||left>maximum.x||top>maximum.y);
    };
    if(intersects(double(size.x)*.5,double(size.y)*.5))return hit();
    return {intersects(24,24)?CollisionKind::Graze:CollisionKind::None,false};
}
PlayerCollisionResult PlayerCollision::circle(Vec2 p,float r,bool probe)const noexcept{
    if constexpr(kPortTitle==14||kPortTitle==16){
        // TH14 44efa0 / TH16 4439e0: JB includes unordered distance; CMOVAE excludes ordered
        // graze boundaries. Invulnerability suppresses hit application only.
        const float dx=position.x-p.x,dy=position.y-p.y;
        const float distance=dx*dx+dy*dy,rr=r*r,hit_radius=radius*radius+rr;
        if(!(distance>=hit_radius)){
            if(dialogue_active)return {};
            if(probe)return {CollisionKind::Graze,false};
            if(state==2||state==3||state==4)return {};
            return {CollisionKind::Hit,invincibility_frames<=0};
        }
        float graze=r/2.5f;if(40.f>graze)graze=40.f;
        const float reach=graze+radius,outer=reach*reach+rr;
        return {distance>=outer?CollisionKind::None:CollisionKind::Graze,false};
    }
    const double dx=double(position.x)-p.x,dy=double(position.y)-p.y;
    const float distance=float(dx*dx+dy*dy);const double squared=double(r)*r;
    // Original TH11 adds the squared radii; it does not square their sum.
    if(double(radius)*radius+squared>distance)return hit();
    float graze=float(double(r)/2.5);if(graze<40)graze=40;
    const double reach=double(radius)+graze;
    return {reach*reach+squared>distance?CollisionKind::Graze:CollisionKind::None,false};
}
PlayerCollisionResult PlayerCollision::laser(Vec2 p,Vec2 half,float angle,float width,float length,bool probe)const noexcept{
    if constexpr(kPortTitle==14||kPortTitle==16){
        // Retail 44f0b0 rounds every multiply before each add/subtract.
        const float dx=position.x-p.x,dy=position.y-p.y;
        const float sine=float(std::sin(-double(angle))),cosine=float(std::cos(-double(angle)));
        const float x=dx*cosine-dy*sine,y=dy*cosine+dx*sine;
        const auto intersects=[&](float scale){
            const float hx=half.x*scale,hy=half.y*scale;
            return !(x-hx>length||y-hy>width*.5f||0.f>x+hx||width*-.5f>y+hy);
        };
        if(!intersects(16))return {};
        if(!intersects(1))return {CollisionKind::Graze,false};
        if((kPortTitle==16?dialogue_active:bomb_active))return {};
        if(probe)return {CollisionKind::Graze,false};
        if(state==2||state==3||state==4||invincibility_frames>0)return {};
        return {CollisionKind::Hit,true};
    }
    const float dx=float(double(position.x)-p.x),dy=float(double(position.y)-p.y);
    const float sine=float(std::sin(-double(angle))),cosine=float(std::cos(-double(angle)));
    const float x=float(double(dx)*cosine-double(sine)*dy),y=float(double(dx)*sine+double(dy)*cosine);
    const auto intersects=[&](float scale){
        const float hx=float(double(half.x)*scale),hy=float(double(half.y)*scale);
        const float left=float(double(x)-hx),right=float(double(x)+hx),top=float(double(y)-hy),bottom=float(double(y)+hy);
        return left<=length&&top<=double(width)*.5&&right>=0&&bottom>=-double(width)*.5;
    };
    if(!intersects(16))return {};
    if(!intersects(1))return {CollisionKind::Graze,false};
    if(probe)return {CollisionKind::Hit,false};
    if(invincibility_frames>0)return {};
    return hit();
}
}
