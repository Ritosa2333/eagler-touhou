#include "ShotDamageTh14.hpp"
#include "ScoreScalarTh14.hpp"
#include <algorithm>
#include <cmath>
#include "Movement.hpp"
namespace th11 {
#if PORT_TITLE == 14
bool th14_damage_circle(std::array<Th14DamageArea,256>& pool,u32& cursor,Vec3 p,float radius,float growth,i32 duration,i32 damage,const float* rate,u32& index)noexcept{
    for(u32 n=0;n<256;++n){if(cursor>=256)cursor=0;auto& a=pool[cursor];if(a.flags&1){++cursor;continue;}
        a.flags=(a.flags&~4u)|3u;a.position=p;a.radius=radius;a.growth=growth;a.timer.set(duration,rate);a.moving=false;
        a.damage=damage;a.total=0;a.maximum=9999999;a.period=1;a.callback=0;a.stamp=0;index=cursor;return true;
    }return false;
}
void th14_damage_tick(Th14DamageArea& a)noexcept{
    if(!(a.flags&1))return;
    if(a.moving){
        auto& m=a.movement;const float rate=a.timer.rate?th14_rate_load(a.timer.rate):1;
        const auto v=polar(m.angle,m.speed*rate);m.velocity.x=v.x;m.velocity.y=v.y;
        m.position.x+=m.velocity.x;m.position.y+=m.velocity.y;m.position.z+=m.velocity.z;
        m.position.x=float(std::floor(double(m.position.x*100.f)))/100.f;m.position.y=float(std::floor(double(m.position.y*100.f)))/100.f;a.position=m.position;
    }else{
        // 44e00f/44e017 run movement for every active area. Even zero-speed
        // death/Bomb areas reach 4040dc's float32 floor-to-centi XY step.
        Movement stationary;stationary.position=a.position;stationary.update();a.position=stationary.position;
    }
    a.radius+=a.growth;a.angle=normalize_angle(a.angle+a.angular_velocity);a.stamp=0;
    auto& t=a.timer;t.previous=t.current;const float rate=t.rate?th14_rate_load(t.rate):1.f;
    t.fractional-=rate>.99f&&rate<1.01f?1.f:rate;t.current=truncate_int(t.fractional);
    if(t.current<=0)a.flags&=~1u;
}
bool th14_damage_rectangle(std::array<Th14DamageArea,256>& pool,u32& cursor,Vec3 p,Vec2 size,float angle,i32 duration,i32 damage,const float* rate,u32& index)noexcept{
    // Retail 450270 uses the same circular search as 450130, but clears
    // the circular/Bomb bits, sets rectangle size and zero angular velocity.
    for(u32 n=0;n<256;++n){if(cursor>=256)cursor=0;auto& a=pool[cursor];if(a.flags&1){++cursor;continue;}
        a.flags=(a.flags&~6u)|1u;a.position=p;a.size=size;a.moving=false;
        constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;u32 loops=0;
        while(angle>pi){angle-=tau;if(loops++>32)break;}while(-pi>angle){angle+=tau;if(loops++>32)break;}
        a.angle=angle;a.angular_velocity=0;a.timer.set(duration,rate);a.damage=damage;a.total=0;a.maximum=9999999;a.period=1;a.callback=0;a.stamp=0;index=cursor;return true;
    }return false;
}
namespace {
Vec2 turn(Vec2 p,float a){if(a==0)return p;const float s=float(std::sin(double(a))),c=float(std::cos(double(a)));return {p.x*c-p.y*s,p.y*c+p.x*s};}
std::array<Vec2,4> corners(Vec3 p,Vec2 size,float angle){const float x=size.x*-.5f,y=size.y*-.5f;std::array<Vec2,4> out={Vec2{x,y},Vec2{x,-y},Vec2{-x,-y},Vec2{-x,y}};for(auto& v:out){v=turn(v,angle);v.x+=p.x;v.y+=p.y;}return out;}
bool inside(Vec2 q,Vec3 p,Vec2 size,float angle){const auto v=turn({q.x-p.x,q.y-p.y},-angle);return size.x*.5f>=std::abs(v.x)&&size.y*.5f>=std::abs(v.y);}
float side(Vec2 a,Vec2 b,Vec2 p){return (p.x-a.x)*(b.y-a.y)+(a.y-p.y)*(b.x-a.x);}
bool segments(Vec2 a,Vec2 b,Vec2 c,Vec2 d){
    const float s=side(a,b,c),t=side(a,b,d);if(s*t>0)return false;
    // lahf/test ah,44/jp takes 406dc8 for ordered nonzero values. Only
    // two zero values reach the native endpoint ordering and bounds checks.
    if(s!=0||t!=0)return side(c,d,a)*side(c,d,b)<=0;
    if(a.x>b.x)std::swap(a,b);if(c.x>d.x)std::swap(c,d);
    return d.x>=a.x&&d.y>=a.y&&b.x>=c.x&&b.y>=c.y;
}
}
bool th14_damage_rectangles(Vec3 p,Vec2 size,float angle,Vec3 q,Vec2 other,float other_angle)noexcept{
    const float dx=p.x-q.x,dy=p.y-q.y,hx=size.x*.5f,hy=size.y*.5f,ox=other.x*.5f,oy=other.y*.5f;
    if(float(std::sqrt(double(dx*dx+dy*dy)))>=float(std::sqrt(double(hx*hx+hy*hy)))+float(std::sqrt(double(ox*ox+oy*oy))))return false;
    const auto a=corners(p,size,angle),b=corners(q,other,other_angle);
    for(const auto v:b)if(inside(v,p,size,angle))return true;
    for(const auto v:a)if(inside(v,q,other,other_angle))return true;
    for(u32 j=0;j<4;++j)for(u32 k=0;k<4;++k)if(segments(a[j],a[(j+1)%4],b[k],b[(k+1)%4]))return true;
    return false;
}
bool th14_damage_circle_rectangle(Vec3 rectangle,Vec2 size,float angle,Vec3 circle,float radius,NativeStackTh14* stack)noexcept{
    // 404ca0: SSE rounding, inclusive strips but strict corner comparisons.
    const float dx=circle.x-rectangle.x,dy=circle.y-rectangle.y;
    const float sine=float(std::sin(-double(angle))),cosine=float(std::cos(-double(angle)));
    const float x=dx*cosine-dy*sine,y=dy*cosine+dx*sine,hx=size.x*.5f,hy=size.y*.5f;
    const float ax=std::abs(x),ay=std::abs(y);
    if(stack)stack->damage_circle_rectangle(ax);
    if(hx+radius>=ax&&hy>=ay)return true;
    if(hx>=ax&&hy+radius>=ay)return true;
    const float r2=radius*radius;
    float a=x-hx,b=y-hy;if(r2>a*a+b*b)return true;
    a=x+hx;if(r2>a*a+b*b)return true;
    b=y+hy;a=x-hx;if(r2>a*a+b*b)return true;
    a=x+hx;return r2>a*a+b*b;
}
bool th14_damage_collect(std::array<Th14DamageArea,256>& areas,Th14DamageWorld& world,Vec3 p,const Vec2* size,float radius,float angle,
    bool frame_advanced,bool probe,u32 argument,u32 stamp,i32 cap,GameEconomy& economy,i32& damage,bool& bomb,Vec3* last_hit,NativeStackTh14* stack)noexcept{
    damage=0;if(!frame_advanced)return true;bomb=false;
    for(u32 j=0;j<areas.size();++j){auto& a=areas[j];if(!(a.flags&1))continue;
        if(a.timer.current==a.timer.previous)continue;
        if(!a.period)return false;if(a.timer.current%a.period)continue;
        bool overlaps=false;
        if(!(a.flags&2)){
            if(size){if(stack)stack->damage_rectangles_call();overlaps=th14_damage_rectangles(a.position,a.size,a.angle,p,*size,angle);}
            else overlaps=th14_damage_circle_rectangle(a.position,a.size,a.angle,p,radius,stack);
        }else if(size)overlaps=th14_damage_circle_rectangle(p,*size,angle,a.position,a.radius,stack);
        else{const float x=a.position.x-p.x,y=a.position.y-p.y,r=a.radius+radius;overlaps=x*x+y*y<=r*r;}
        if(!overlaps||(stamp&&a.stamp==stamp))continue;if(stamp)a.stamp=stamp;if(a.flags&4)bomb=true;
        i32 hit=a.damage;
        if(!probe){if(a.callback){a.argument=argument;if(!world.hit(j,a,p,size,radius,angle,hit))return false;}a.total=wrapping_add(a.total,a.damage);}
        damage=wrapping_add(damage,hit);if(last_hit)*last_hit=a.position;
        if(a.maximum<9999999&&a.total>=a.maximum){a.flags&=~1u;a.damage=0;}
    }
    if(damage>cap)damage=cap;
    if(!probe&&damage){
        // Native score is in ten-point units and applies two signed divisions.
        th14_damage_score_words(damage,[&](){return th14_hud_load(economy.score_units);},
            [&](i32 value){th14_hud_store(economy.score_units,value);});
    }
    return true;
}
#endif
}
