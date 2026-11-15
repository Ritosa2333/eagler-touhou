#include "BombReimuTh14.hpp"
#include "Movement.hpp"
#include <cmath>
namespace th11 {
#if PORT_TITLE == 14
namespace {
Enemy* nearest_orb(EnemyLink* list,Vec3 p){
    Enemy* found=nullptr;float limit=512.f*512.f;
    for(auto* n=list;n;n=n->next){auto* e=n->value;if(e->state.flags&0xc000021u)continue;
        const float x=p.x-e->state.current.position.x,y=p.y-e->state.current.position.y,d=x*x+y*y;
        if(limit>d){found=e;limit=d;}}
    return found;
}
float angle_difference(float target,float current){
    const float d=target-current;if(d>3.1415927410125732f)return target-(current+6.2831854820251465f);
    if(current-target>3.1415927410125732f)return target-(current-6.2831854820251465f);return d;
}
}
void th14_reimu_orb_tick(Th14ReimuOrb& o,Vec3 player,EnemyLink* enemies,bool manager_present,const float* rate)noexcept{
    if(o.timer.current!=o.timer.previous){
        const i32 detach=i32((o.index+9)*10);
        if(o.timer.current<90){o.anchor=player;o.radius+=1.5f;o.angle=normalize_angle(o.angle+.10471975803375244f);}
        else if(o.timer.current<detach){o.anchor=player;o.angle=normalize_angle(o.angle+.10471975803375244f);}
        else if(o.timer.current==detach){
            o.flags&=~15u;o.angle=normalize_angle(float(std::atan2(double(o.last_delta.y),double(o.last_delta.x))));
            o.speed=float(std::sqrt(double(o.last_delta.x*o.last_delta.x+o.last_delta.y*o.last_delta.y)));
        }else{
            if(manager_present)o.target=nearest_orb(enemies,o.position);
            if(o.target){
                if(!(o.target->state.flags&0xc000021u)){
                    const float x=o.target->state.current.position.x-o.position.x,y=o.target->state.current.position.y-o.position.y;
                    const float d=angle_difference(float(std::atan2(double(y),double(x))),o.angle),absolute=std::abs(d);
                    float speed=o.speed;
                    if(absolute>=.7853981852531433f){speed-=.699999988079071f;if(1.f>speed)speed=1;}
                    else if(.2617993950843811f>absolute){speed+=.20000000298023224f;if(speed>8)speed=8;}
                    o.angle=normalize_angle(normalize_angle(o.angle+d*.10000000149011612f));o.speed=speed;
                }
            }else if(o.position.x< -160||o.position.x>160||o.position.y<32||o.position.y>416)o.speed*=.8999999761581421f;
        }
    }
    const Vec3 before=o.position;const float speed=th14_rate_load(rate);
    if((o.flags&15)==0){
        const auto v=polar(o.angle,o.speed*speed);o.anchor.x=v.x;o.anchor.y=v.y;
        o.position.x+=o.anchor.x;o.position.y+=o.anchor.y;o.position.z+=o.anchor.z;
    }else if((o.flags&15)==2){
        o.radius+=o.turn*speed;o.angle=normalize_angle(o.angle+o.speed*speed);
        const auto v=polar(o.angle,o.radius);o.position={o.anchor.x+v.x,o.anchor.y+v.y,o.anchor.z};
    }
    o.position.x=float(std::floor(double(o.position.x*100.f)))/100.f;
    o.position.y=float(std::floor(double(o.position.y*100.f)))/100.f;
    o.last_delta={o.position.x-before.x,o.position.y-before.y,o.position.z-before.z};
    if(!o.timer.rate)o.timer.rate=rate;o.timer.tick();
}
#endif
}
