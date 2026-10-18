#include "BulletLaunch.hpp"
#include "Movement.hpp"
namespace th11 {
bool bullet_launch(const BulletEmitter& e,i32 index,i32 layer,float aim,Rng& rng,
                   const Vec3& player,float exclusion,BulletLaunch& out)noexcept{
    constexpr double pi=3.1415927410125732421875,tau=6.283185482025146484375;
    float speed=e.layers>1?float(double(e.speed)-(double(e.speed)-e.slow_speed)*layer/e.layers):e.speed;
    float angle=0;
    const auto random_between=[&](float high,float low){const float delta=float(double(high)-low),unit=rng.unit(),product=float(double(unit)*delta);return float(double(low)+product);};
    switch(e.aim){
    case 0:case 1:
        angle=(e.count&1)?float(double(wrapping_add(index,1)/2)*e.spread+0.):float(double(index/2)*e.spread+double(e.spread)*.5+0.);
        if(index&1)angle=float(double(angle)*-1.);
        if(e.aim==0)angle=float(double(angle)+aim);
        angle=float(double(e.angle)+angle);break;
    case 2:case 3:{
        if(e.aim==2)angle=float(double(aim)+0.);
        const float ring=float(double(index)*tau/e.count+angle);
        angle=float(double(e.spread)*layer+e.angle+ring);break;
    }
    case 4:case 5:{
        if(e.aim==4)angle=float(double(aim)+0.);
        const float half=float(pi/e.count+angle),ring=float(double(index)*tau/e.count+half);
        angle=float(double(e.spread)*layer+e.angle+ring);break;
    }
    case 6:angle=random_between(e.angle,e.spread);break;
    case 7:{
        speed=random_between(e.speed,e.slow_speed);
        const float ring=float(double(index)*tau/e.count+0.);
        angle=float(double(e.spread)*layer+e.angle+ring);break;
    }
    case 8:angle=random_between(e.angle,e.spread);speed=random_between(e.speed,e.slow_speed);break;
    default:break;
    }
    out.speed=speed;out.angle=normalize_angle(angle);out.position=e.position;
    if(e.radius!=0){const auto offset=polar(out.angle,e.radius);out.position.x=float(double(out.position.x)+offset.x);out.position.y=float(double(offset.y)+out.position.y);}
    out.position.z=.1f;
    if(exclusion>0){const double dx=double(out.position.x)-player.x,dy=double(out.position.y)-player.y;const float squared=float(dx*dx+dy*dy);if(squared<exclusion)return false;}
    // The original stores the normalized angle but uses the pre-normalized value
    // for velocity. Reducing it first can alter both components by several ULPs.
    out.velocity=polar(angle,speed);return true;
}
}
