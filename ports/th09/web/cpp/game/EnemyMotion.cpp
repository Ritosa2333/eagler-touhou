#include "EnemyMotion.hpp"
#include "EclVm.hpp"
#include "GameMath.hpp"
#include <cmath>
namespace th09 {
namespace {
constexpr float pi=3.1415927410125732f,half_pi=1.5707963705062866f;
Vec2 polar(float angle,float radius){return {float(std::cos(double(angle))*double(radius)),float(std::sin(double(angle))*double(radius))};}
float direction_to(const Vec3& from,const Vec3& to){const float x=to.x-from.x,y=to.y-from.y;return x==0&&y==0?half_pi:float(std::atan2(double(y),double(x)));}
float ease(float t,u32 mode){
    if(mode>=1&&mode<=3){float v=t*t;for(u32 n=1;n<mode;++n)v*=t;return v;}
    if(mode>=4&&mode<=6){const float r=1.f-t;float v=r*r;for(u32 n=4;n<mode;++n)v*=r;return 1.f-v;}return t;
}
}
void EnemyMotion::clamp(EclVm& vm) noexcept {
    if(!(vm.behavior_flags&0x10000))return;auto& p=vm.values.position;
    if(p.x<bounds[0])p.x=bounds[0];else if(p.x>bounds[2])p.x=bounds[2];
    if(p.y<bounds[1])p.y=bounds[1];else if(p.y>bounds[3])p.y=bounds[3];
}
bool EnemyMotion::command(EclVm& vm,const EclInstruction& i){
    auto& v=vm.values;const auto iv=[&](u32 n){return vm.integer(i,n);};const auto fv=[&](u32 n){return vm.real(i,n);};
    const auto polar_mode=[&]{vm.behavior_flags=(vm.behavior_flags&~0x400u)|0x200;};
    const auto timed_heading=[&](float angle,u32 speed_arg,bool mirror){
        float speed=fv(speed_arg);i32 frames=iv(0);v.target.x=float(std::cos(double(angle))*double(frames))*speed;
        speed=fv(speed_arg);frames=iv(0);v.target.y=float(std::sin(double(angle))*double(frames))*speed;v.target.z=0;v.origin=v.resolved_position;
        duration=iv(0);time.reset(duration);const u32 easing=u32(iv(1));vm.behavior_flags=(vm.behavior_flags&0xffffc5ffu)|((easing&7)<<11)|0x400;
        if(mirror&&(vm.behavior_flags&0x8000))v.target.x=-v.target.x;
    };
    switch(i.opcode){
    case 63:v.position.x=fv(0);v.position.y=fv(1);v.position.z=0;clamp(vm);break;
    case 64:{
        const float x=fv(2),y=fv(3);v.target={x-v.resolved_position.x,y-v.resolved_position.y,0.f-v.resolved_position.z};v.origin=v.position;
        duration=iv(0);time.reset(duration);const u32 easing=u32(iv(1));const u32 flags=vm.behavior_flags;
        vm.behavior_flags=(flags&0xffffc5ffu)|((easing&7)<<11)|0x400;vm.velocity={};if(flags&0x8000)v.target.x=-v.target.x;break;
    }
    case 65:v.direction=float(add_angle(fv(0),0));v.speed=fv(1);polar_mode();duration=0;time.reset();break;
    case 66:case 69:{
        const i32 frames=iv(0);
        if(frames>0){const float a=float(add_angle(fv(2),0));timed_heading(a,3,true);}
        else {
            float a=fv(2);if(i.opcode==69){if(!v.field)return false;a=float(add_angle(a,direction_to(v.position,v.field->player)));}else a=float(add_angle(a,0));
            v.direction=a;v.speed=fv(3);polar_mode();duration=i.opcode==69?iv(0):0;time.reset(duration);
        }break;
    }
    case 67:case 178:{
        if(!v.world||!v.field)return false;auto& rng=v.world->random;float a;
        if(i.opcode==178){
            if(rng.bounded32(4)==0)a=float(rng.signed_unit())*pi;
            else if(v.position.x<=v.field->player.x){
                if(v.position.x-(v.field->player.x-384.f)<=v.field->player.x-v.position.x)a=float(add_angle(float(rng.range(half_pi))+2.356194496154785f,0));
                else a=float(rng.range(half_pi))-.7853981852531433f;
            }else if((v.field->player.x+384.f)-v.position.x<=v.position.x-v.field->player.x)a=float(add_angle(float(rng.range(half_pi))-.7853981852531433f,0));
            else a=float(add_angle(float(rng.range(half_pi))+2.356194496154785f,0));
        }else {
            a=v.position.x<=v.field->player.x?float(rng.range(half_pi))-.7853981852531433f:float(add_angle(float(rng.range(half_pi))+2.356194496154785f,0));
            if(v.position.x<bounds[0]+96.f){if(a>half_pi)a=pi-a;else if(a<-half_pi)a=-pi-a;}
            if(bounds[2]-96.f<v.position.x){
                if(a>=0&&a<half_pi)a=pi-v.direction;
                else if(a>-half_pi&&a<0)a=-pi-a;
            }
        }
        if(v.position.y<bounds[1]+48.f&&a<0)a=-a;
        if(bounds[3]-48.f<v.position.y&&a>0)a=-a;
        if(iv(0)>0)timed_heading(a,2,false);
        else {v.direction=a;v.speed=fv(2);polar_mode();duration=0;time.reset();}break;
    }
    case 68:{if(!v.field)return false;const float a=fv(0);v.direction=float(add_angle(a,direction_to(v.position,v.field->player)));v.speed=fv(1);break;}
    case 70:v.angular_velocity=fv(0);polar_mode();break;
    case 71:v.acceleration=fv(0);polar_mode();break;
    case 72:duration=iv(0);time.reset(duration);v.origin.x=fv(1);v.origin.y=fv(2);v.orbit_angle=fv(3);v.orbit_velocity=fv(4);v.orbit_radius=fv(5);orbit_growth=fv(6);vm.behavior_flags|=0x600;break;
    case 73:duration=iv(0);time.reset(duration);v.origin=v.position;v.orbit_angle=fv(1);v.orbit_velocity=fv(2);v.orbit_radius=0;orbit_growth=fv(3);vm.behavior_flags|=0x600;break;
    case 74:duration=iv(0);time.reset(duration);v.orbit_velocity=fv(1);orbit_growth=fv(2);vm.behavior_flags|=0x600;break;
    case 75:for(u32 n=0;n<4;++n)bounds[n]=fv(n);vm.behavior_flags|=0x10000;break;
    case 76:vm.behavior_flags&=~0x10000u;break;
    case 77:hitbox.x=fv(0);hitbox.y=fv(1);break;
    case 78:low_damage_hitbox.x=fv(0);low_damage_hitbox.y=fv(1);break;
    case 79:{const u32 flags=u32(iv(0));vm.behavior_flags=(vm.behavior_flags&0xfdffffa3u)|((flags&16)?0x2000000:0)|((flags&8)?16:0)|((flags&1)?0:64)|((flags&2)?0:4)|((flags&4)?0:8);
        v.flags=(v.flags&~8u)|((flags>>2)&8);break;}
    case 80:case 81:{const u32 flags=u32(iv(0));const bool enable=i.opcode==80;
        const auto bit=[&](u32 b,bool state){if(state)vm.behavior_flags|=b;else vm.behavior_flags&=~b;};
        if(flags&1)bit(64,!enable);if(flags&2)bit(4,!enable);if(flags&4)bit(8,!enable);if(flags&8)bit(16,enable);if(flags&16)bit(0x2000000,enable);
        if(flags&32){if(enable)v.flags|=8;else v.flags&=~8u;}break;}
    case 82:{const double radius=vm.wide(i,0);player_protect_squared=float(radius*radius);break;}
    default:return false;
    }
    return !vm.invalid;
}
void EnemyMotion::update_velocity(EclVm& vm,const FrameTiming& timing,float frame_step) noexcept {
    auto& v=vm.values;switch((vm.behavior_flags>>9)&3){
    case 1:{
        v.direction=float(add_angle(v.direction,timing.rate*v.angular_velocity));v.speed=timing.rate*v.acceleration+v.speed;
        const auto d=polar(v.direction,v.speed);vm.velocity={d.x,d.y,0};
        if(duration>0){time.decrement(1,timing);if(time.current<=0)vm.behavior_flags&=~0x600u;}break;
    }
    case 2:{
        time.decrement(1,timing);float fraction=1.f-time.time/float(duration);if(fraction<0)fraction=0;
        fraction=ease(fraction,(vm.behavior_flags>>11)&7);
        const Vec3 d{v.target.x*fraction,v.target.y*fraction,v.target.z*fraction};
        vm.velocity={(v.origin.x+d.x)-v.position.x,(v.origin.y+d.y)-v.position.y,(v.origin.z+d.z)-v.position.z};
        if(vm.behavior_flags&0x8000)vm.velocity.x=-vm.velocity.x;
        v.direction=float(std::atan2(double(vm.velocity.y),double(vm.velocity.x)));
        if(time.current<=0){vm.behavior_flags&=~0x600u;v.position={v.origin.x+v.target.x,v.origin.y+v.target.y,v.origin.z+v.target.z};vm.velocity={};}break;
    }
    case 3:{
        v.orbit_angle=float(add_angle(v.orbit_angle,timing.rate*v.orbit_velocity));v.orbit_radius=(frame_step*orbit_growth)*timing.rate+v.orbit_radius;
        const auto d=polar(v.orbit_angle,v.orbit_radius);vm.velocity.x=(d.x+v.origin.x)-v.position.x;vm.velocity.y=(d.y+v.origin.y)-v.position.y;
        v.direction=float(std::atan2(double(vm.velocity.y),double(vm.velocity.x)));
        if(duration>0){time.advance(-frame_step,timing.rate,timing.force_step?32:0);if(time.current<=0)vm.behavior_flags&=~0x600u;}break;
    }
    }
}
void EnemyMotion::integrate_position(EclVm& vm,const FrameTiming& timing)noexcept{
    auto& v=vm.values;auto& p=v.position;v.last_delta={p.x-previous_position.x,p.y-previous_position.y,p.z-previous_position.z};previous_position=p;
    if(v.flags&0x1000){p.x=timing.rate*capture_velocity.x+p.x;p.y=timing.rate*capture_velocity.y+p.y;return;}
    const float vx=timing.rate*vm.velocity.x;p.x=vm.behavior_flags&0x8000?p.x-vx:vx+p.x;p.y=timing.rate*vm.velocity.y+p.y;
}
}
