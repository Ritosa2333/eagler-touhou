#include "PlayerCollision.hpp"
#include <cmath>
namespace th11 {
PlayerCollisionResult PlayerCollision::hit()const noexcept{
    if(bomb_active||state==2||state==3||state==4||(flags&2))return {};
    return {CollisionKind::Hit,invincibility_frames<=0};
}
PlayerCollisionResult PlayerCollision::rectangle(Vec2 p,Vec2 size)const noexcept{
    // The original stores the four rectangle edges as float before comparing.
    const auto intersects=[&](double half_x,double half_y){
        const float left=float(double(p.x)-half_x),right=float(double(p.x)+half_x);
        const float top=float(double(p.y)-half_y),bottom=float(double(p.y)+half_y);
        return !(right<minimum.x||bottom<minimum.y||left>maximum.x||top>maximum.y);
    };
    if(intersects(double(size.x)*.5,double(size.y)*.5))return hit();
    return {intersects(24,24)?CollisionKind::Graze:CollisionKind::None,false};
}
PlayerCollisionResult PlayerCollision::circle(Vec2 p,float r)const noexcept{
    const double dx=double(position.x)-p.x,dy=double(position.y)-p.y;
    const float distance=float(dx*dx+dy*dy);const double squared=double(r)*r;
    // Original TH11 adds the squared radii; it does not square their sum.
    if(double(radius)*radius+squared>distance)return hit();
    float graze=float(double(r)/2.5);if(graze<40)graze=40;
    const double reach=double(radius)+graze;
    return {reach*reach+squared>distance?CollisionKind::Graze:CollisionKind::None,false};
}
PlayerCollisionResult PlayerCollision::laser(Vec2 p,Vec2 half,float angle,float width,float length,bool probe)const noexcept{
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
