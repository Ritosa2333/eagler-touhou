#include "PlayerCollision.hpp"
#include <cmath>
namespace th15 {
PlayerContact PlayerCollision::laser(const Vec2& origin,float angle,float length,float width,bool graze_only,PlayerDamageHost* damage)const{
    const float dx=float(position.x-origin.x),dy=float(position.y-origin.y),s=float(std::sin(double(-angle))),c=float(std::cos(double(-angle)));
    const float x=float(float(c*dx)-float(dy*s)),y=float(float(dy*c)+float(s*dx)),hx=laser_half_size.x,hy=laser_half_size.y,upper=float(width*.5f),lower=float(width*-.5f);
    if(float(x-float(hx*16.f))>length||float(y-float(hy*16.f))>upper||float(float(hx*16.f)+x)<0||float(float(hy*16.f)+y)<lower)return PlayerContact::none;
    if(float(x-hx)>length||float(y-hy)>upper||float(x+hx)<0||float(hy+y)<lower)return PlayerContact::graze;
    if(bomb_active)return PlayerContact::none;if(graze_only)return PlayerContact::graze;if(state==2||state==4||state==3||invulnerability>0)return PlayerContact::none;if(damage)damage->hit();return PlayerContact::hit;
}
PlayerContact PlayerCollision::contact(bool graze_only,PlayerDamageHost* damage)const{
    if(bomb_active)return PlayerContact::none;if(graze_only)return PlayerContact::graze;
    if(state==2||state==3||state==4)return PlayerContact::none;
    if(invulnerability<1&&damage)damage->hit();return PlayerContact::hit;
}
PlayerContact PlayerCollision::rectangle(const Vec2& p,const Vec2& size,bool graze_only,PlayerDamageHost* damage)const{
    const float left=float(p.x-float(size.x*.5f)),top=float(p.y-float(size.y*.5f)),right=float(p.x+float(size.x*.5f)),bottom=float(p.y+float(size.y*.5f));
    if(right<hitbox_min.x||bottom<hitbox_min.y||(hitbox_max.x<=left&&left!=hitbox_max.x)||(hitbox_max.y<=top&&top!=hitbox_max.y)){
        if(hitbox_min.x<=float(p.x+24.f)&&hitbox_min.y<=float(p.y+24.f)&&(float(p.x-24.f)<hitbox_max.x||float(p.x-24.f)==hitbox_max.x)&&(float(p.y-24.f)<hitbox_max.y||float(p.y-24.f)==hitbox_max.y))return PlayerContact::graze;
        return PlayerContact::none;
    }
    return contact(graze_only,damage);
}
PlayerContact PlayerCollision::circle(const Vec2& p,float bullet_radius,bool graze_only,PlayerDamageHost* damage)const{
    const float x=float(position.x-p.x),y=float(position.y-p.y),distance=float(float(x*x)+float(y*y));float hit_radius=radius;if(enlarged)hit_radius=float(float(size_multiplier*3.6f)*hit_radius);
    const float bullet_squared=float(bullet_radius*bullet_radius);
    if(float(float(hit_radius*hit_radius)+bullet_squared)<=distance){float graze=float(bullet_radius/2.5f);if(graze<40)graze=40;const float extent=float(graze+hit_radius);return float(float(extent*extent)+bullet_squared)<=distance?PlayerContact::none:PlayerContact::graze;}
    return contact(graze_only,damage);
}
}
