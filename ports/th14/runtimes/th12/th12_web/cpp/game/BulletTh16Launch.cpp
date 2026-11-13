#include "BulletLaunch.hpp"
#include "EnemyTh16Emitter.hpp"
#include "Movement.hpp"
#include <cmath>
namespace th11 {
bool bullet_th16_launch(const Th16BulletEmitter& e,i32 index,i32 layer,float aim,Rng& rng,
                       const Vec3& player,float exclusion,BulletLaunch& out)noexcept{
    constexpr float pi=3.1415927410125732421875f,tau=6.283185482025146484375f;
    float speed=e.layers>1?e.speed-((e.speed-e.slow_speed)*float(layer))/float(e.layers-1):e.speed;
    float angle=0;
    switch(e.aim){
    case 0:case 1:{
        float spread=(e.count&1)?float(wrapping_add(index,1)/2)*e.spread:float(index/2)*e.spread+e.spread*.5f;
        spread+=0.f;if(index&1)spread*=-1.f;if(e.aim==0)spread+=aim;angle=e.angle+spread;break;
    }
    case 2:case 3:case 4:case 5:{
        if(e.aim==2||e.aim==4)angle=aim+0.f;
        if(e.aim==4||e.aim==5)angle+=pi/float(e.count);
        angle+=(float(index)*tau)/float(e.count);angle+=float(layer)*e.spread+e.angle;break;
    }
    case 6:angle=e.angle+rng.signed_unit()*e.spread;break;
    case 7:
        speed=rng.unit()*e.slow_speed+e.speed;
        angle=(float(index)*tau)/float(e.count)+0.f;angle+=float(layer)*e.spread+e.angle;break;
    case 8:
        angle=e.angle+rng.signed_unit()*e.spread;speed=rng.unit()*e.slow_speed+e.speed;break;
    case 9:case 10:{
        const float ring=(float(index)*tau)/float(e.count);
        const bool odd=(e.layers&1)!=0;
        float spread=odd?float(wrapping_add(layer,1)/2)*e.spread+ring:(float(layer/2)*e.spread+e.spread*.5f)+ring;
        if(e.layers>1){const float step=float((odd?wrapping_add(layer,1):layer)&0xfffe);speed=((e.slow_speed-e.speed)*step)/float(e.layers-1)+e.speed;}
        if(layer&1)spread*=-1.f;if(e.aim==9)spread+=aim;angle=e.angle+spread;break;
    }
    case 11:case 12:{
        float ring=(float(index)*tau)/float(e.count);if(e.aim==12)ring+=pi/float(e.count);
        angle=(e.angle+ring)+0.f;
        speed=(1.f-std::abs(float(std::sin(double(ring))))*e.slow_speed)*speed;break;
    }
    default:break;
    }
    out.speed=speed;out.angle=normalize_angle(normalize_angle(angle+0.f));out.position=e.position;
    if(e.radius!=0){const auto offset=polar(out.angle,e.radius);out.position.x+=offset.x;out.position.y=offset.y+out.position.y;}
    out.position.z=.1f;
    if(exclusion>0){const float dx=out.position.x-player.x,dy=out.position.y-player.y;if(exclusion>dx*dx+dy*dy)return false;}
    out.velocity=polar(angle,speed);return true;
}
}
