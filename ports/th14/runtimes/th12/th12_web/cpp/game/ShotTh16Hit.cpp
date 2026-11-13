#include "ShotManager.hpp"
#include <algorithm>
#include <cmath>
#if PORT_TITLE == 16
namespace th11 {
void shot_th16_beam_effect_setup(AnmVm& vm,const Th16BeamHit& hit,Vec3 start,const float* rate){
    vm.flags|=4;vm.rotation.z=hit.angle;auto& v=vm.position_interpolation;
    v.duration=20;v.mode=InterpolationMode::Decelerate2;v.timer.set(0,rate);
    std::memcpy(v.start,&start,12);std::memcpy(v.end,&hit.travel,12);
    std::memcpy(v.initial_tangent,&start,12);std::memcpy(v.final_tangent,&start,12);
}
float shot_th16_freeze_angle(const Th16ShotSpawn& shot,Rng& random){return normalize_angle(shot.movement.angle+random.signed_unit()*.3490658402442932f);}
void shot_th16_freeze_finish(AnmVm& vm,Rng& random,const float* rate){
    auto& scale=vm.scale_interpolation;scale.duration=20;scale.mode=InterpolationMode::Linear;
    scale.start[0]=scale.start[1]=1;scale.end[0]=scale.end[1]=3;scale.timer.set(0,rate);
    const u32 red=(random.next32()&127)+127,green=(random.next32()&127)+64,blue=(random.next32()&63)+64;
    vm.color=(vm.color&0xff000000u)|(red<<16)|(green<<8)|blue;
}
bool shot_th16_beam_hit(Th16ShotSpawn& shot,Th16DamageSource& area,const Th16DamageQuery& query,Th16BeamHit& out){
    out={};shot.hit_this_frame=1;if(!shot.hit_last_frame){out.interrupt=true;shot.hit_last_frame=1;}
    const auto origin=shot.movement.position;const float dx=query.position.x-origin.x,dy=query.position.y-origin.y;
    const float height=float_bits(area.words[6]);
    if(!query.size){
        const float radius=height*.5f+query.radius,s=float(std::sin(-double(shot.movement.angle))),c=float(std::cos(-double(shot.movement.angle)));
        const float x=dx*c-dy*s,y=dy*c+dx*s;float distance=x;
        if(std::abs(y)<=radius&&x>=-radius&&(x>=0||x*x+y*y<=radius*radius)){
            if(radius==0)return false;const float part=y/radius;distance=x-float(std::sqrt(double(1.f-part*part)))*radius;
        }
        shot.hitbox.x=std::max(0.f,distance+8.f);area.words[5]=float_bits(shot.hitbox.x);
    }else{
        Vec2 point;const Vec2 expanded{query.size->x+height,query.size->y+height};
        if(shot_th16_beam_intersection(origin,query.position,expanded,query.angle,shot.movement.angle,point)){
            const float x=point.x-origin.x,y=point.y-origin.y,aim=float(std::atan2(double(y),double(x)));float delta=aim-shot.movement.angle;
            if(delta>3.1415927410125732f)delta=aim-(shot.movement.angle+6.2831854820251465f);
            else if(shot.movement.angle-aim>3.1415927410125732f)delta=aim-(shot.movement.angle-6.2831854820251465f);
            shot.hitbox.x=std::abs(delta)<1.5707963705062866f?float(std::sqrt(double(y*y+x*x)))+8.f:8.f;
            shot.hitbox.x=std::max(0.f,shot.hitbox.x);area.words[5]=float_bits(shot.hitbox.x);
        }else{shot.hitbox.x=std::max(0.f,float(std::sqrt(double(dx*dx+dy*dy)))-24.f);area.words[5]=float_bits(shot.hitbox.x+16.f);}
    }
    if(shot.timer.current!=shot.timer.previous){
        if(shot.timer.current%2==0){out.effect=true;out.angle=shot.movement.angle;const auto endpoint=polar(out.angle,shot.hitbox.x),travel=polar(out.angle,64);out.position={endpoint.x+origin.x,endpoint.y+origin.y,0};out.travel={travel.x,travel.y,0};}
        if(shot.timer.current%4==0)out.damage=signed_bits(area.words[29]);
    }return true;
}
}
#endif

