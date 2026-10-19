#include "DamageSources.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
namespace {
Vec2 rotate(Vec2 p,float angle){const float sine=float(std::sin(double(angle))),cosine=float(std::cos(double(angle)));return {float(float(p.x*cosine)-float(p.y*sine)),float(float(p.y*cosine)+float(p.x*sine))};}
std::array<Vec2,4> corners(Vec2 center,Vec2 size,float angle){std::array<Vec2,4> p={Vec2{float(size.x*-.5f),float(size.y*-.5f)},Vec2{float(size.x*-.5f),float(size.y*.5f)},Vec2{float(size.x*.5f),float(size.y*.5f)},Vec2{float(size.x*.5f),float(size.y*-.5f)}};for(auto& v:p){if(angle!=0)v=rotate(v,angle);v.x=float(v.x+center.x);v.y=float(v.y+center.y);}return p;}
bool contains_corner(const std::array<Vec2,4>& p,Vec2 center,Vec2 size,float angle){for(auto v:p){v.x=float(v.x-center.x);v.y=float(v.y-center.y);if(angle!=0)v=rotate(v,-angle);if(std::abs(v.x)<=float(size.x*.5f)&&std::abs(v.y)<=float(size.y*.5f))return true;}return false;}
}
bool rectangle_circle(const Vec2& center,const Vec2& size,float angle,const Vec2& other,float radius){
    const Vec2 local=rotate({float(other.x-center.x),float(other.y-center.y)},-angle);const float hx=float(size.x*.5f),hy=float(size.y*.5f);
    if(!((float(hx+radius)<std::abs(local.x)||hy<std::abs(local.y))&&(hx<std::abs(local.x)||float(hy+radius)<std::abs(local.y))))return true;
    const float x0=float(local.x-hx),x1=float(hx+local.x),y0=float(local.y-hy),y1=float(hy+local.y),r2=float(radius*radius);
    return float(float(x0*x0)+float(y0*y0))<r2||float(float(x1*x1)+float(y0*y0))<r2||float(float(x0*x0)+float(y1*y1))<r2||float(float(x1*x1)+float(y1*y1))<r2;
}
bool rectangle_rectangle(const Vec2& center,const Vec2& size,float angle,const Vec2& other,const Vec2& other_size,float other_angle){
    const float dx=float(center.x-other.x),dy=float(center.y-other.y),a=float(size.x*.5f),b=float(size.y*.5f),c=float(other_size.x*.5f),d=float(other_size.y*.5f);
    const float distance=float(std::sqrt(double(float(float(dy*dy)+float(dx*dx))))),r0=float(std::sqrt(double(float(float(b*b)+float(a*a))))),r1=float(std::sqrt(double(float(float(c*c)+float(d*d)))));
    if(!(distance<float(r0+r1)))return false;
    const auto first=corners(center,size,angle),second=corners(other,other_size,other_angle);
    if(contains_corner(second,center,size,angle)||contains_corner(first,other,other_size,other_angle))return true;
    for(u32 i=0;i<4;i++)for(u32 j=0;j<4;j++){
        const auto p=first[i],q=first[(i+1)%4],r=second[j],s=second[(j+1)%4];const float x=float(p.x-q.x),y=float(p.y-q.y);
        const float u=float(float(float(p.x-r.x)*y)+float(float(r.y-p.y)*x)),v=float(float(float(p.x-s.x)*y)+float(float(s.y-p.y)*x));
        if(float(u*v)>0)continue;
        if(u==0&&v==0){float px=p.x,qx=q.x,py=p.y,qy=q.y,rx=r.x,sx=s.x,ry=r.y,sy=s.y;if(qx<px){std::swap(px,qx);std::swap(py,qy);}if(sx<rx){std::swap(rx,sx);std::swap(ry,sy);}if(px<=sx&&py<=sy&&rx<=qx&&ry<=qy)return true;}
        else{const float a0=float(float(float(r.x-p.x)*float(r.y-s.y))+float(float(p.y-r.y)*float(r.x-s.x))),a1=float(float(float(r.x-q.x)*float(r.y-s.y))+float(float(q.y-r.y)*float(r.x-s.x)));if(float(a0*a1)<=0)return true;}
    }return false;
}
i32 DamageSources::allocate(){for(i32 attempt=0;;attempt++){cursor++;if(cursor>255)cursor=0;if(!(sources[cursor].flags&1))return cursor+1;if(attempt>=255)return -(cursor+1);}}
i32 DamageSources::circle(const Vec3& p,float radius,float growth,i32 lifetime,i32 damage){
    const i32 id=allocate();if(id<0)return -id;auto& s=sources[id-1];s.flags=(s.flags&~4u)|3;s.motion={};s.motion.position=p;s.radius=radius;s.radius_growth=growth;s.lifetime.set(lifetime);s.damage=damage;s.accumulated=0;s.damage_limit=9999999;s.interval=1;s.callback_kind=0;s.last_target=0;return id;
}
i32 DamageSources::rectangle(const Vec3& p,float angle,const Vec2& size,i32 lifetime,i32 damage){
    const i32 id=allocate();if(id<0)return -id;auto& s=sources[id-1];s.flags=(s.flags&~6u)|1;s.motion={};s.motion.position=p;s.size=size;s.angle=normalize_angle(angle);s.angular_speed=0;s.lifetime.set(lifetime);s.damage=damage;s.accumulated=0;s.damage_limit=9999999;s.interval=1;s.callback_kind=0;s.last_target=0;return id;
}
void DamageSources::tick(float rate){for(auto& s:sources)if(s.flags&1){s.motion.integrate(rate);s.motion.advance(rate);s.radius=float(s.radius+s.radius_growth);s.angle=normalize_angle(float(s.angle+s.angular_speed));s.last_target=0;s.lifetime.decrement(&rate);if(s.lifetime.current<1)s.flags&=~1u;}}
DamageResult DamageSources::query(const DamageQuery& q,bool frame_changed,i32 bomb_damage){
    DamageResult out;if(!frame_changed)return out;out.amount=bomb_damage;out.direct=bomb_damage>0;
    for(auto& s:sources){if(!(s.flags&1)||s.lifetime.current==s.lifetime.previous||s.interval==0||s.lifetime.current%s.interval!=0)continue;const Vec2 p={s.motion.position.x,s.motion.position.y},target={q.position.x,q.position.y};bool hit;
        if(!(s.flags&2))hit=q.rectangle?rectangle_rectangle(p,s.size,s.angle,target,q.size,q.angle):rectangle_circle(p,s.size,s.angle,target,q.radius);
        else if(q.rectangle)hit=rectangle_circle(target,q.size,q.angle,p,s.radius);
        else{const float r=float(s.radius+q.radius),x=float(p.x-target.x),y=float(p.y-target.y);hit=!(float(r*r)<float(float(y*y)+float(x*x)));}
        if(!hit||(q.target!=0&&s.last_target==q.target))continue;if(q.target!=0)s.last_target=q.target;if(s.flags&4)out.direct=true;
        i32 amount=s.damage;if(!q.preview){if(s.callback_kind!=0){if(!hit_callback){error="Damage-source hit callback unavailable";out.amount=-1;return out;}s.callback_target=q.target;amount=hit_callback(s,q);if(!error.empty()){out.amount=-1;return out;}}s.accumulated=wrapping_add(s.accumulated,s.damage);}
        out.amount=wrapping_add(out.amount,amount);out.position=s.motion.position;out.position_written=true;if(s.damage_limit<9999999&&s.damage_limit<=s.accumulated){s.flags&=~1u;s.damage=0;}
    }if(maximum_damage<out.amount)out.amount=maximum_damage;if(!q.preview&&out.amount!=0){score=wrapping_add(score,(out.amount/10+10)/10);if(score>999999999)score=999999999;}return out;
}
}
