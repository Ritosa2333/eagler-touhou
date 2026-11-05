#include "EclVm.hpp"
#include "GameMath.hpp"
#include <cmath>
namespace th08 {
namespace {
constexpr float pi=3.1415927410125732f,half_pi=1.5707963705062866f;
bool less_equal(Extended a,Extended b){return a<b||a==b;}
float angle(float x,float y){return Extended::from_double(std::atan2(double(y),double(x))).to_float();}
Vec3 add(Vec3 a,Vec3 b){return {Scalar::add(a.x,b.x),Scalar::add(a.y,b.y),Scalar::add(a.z,b.z)};}
Vec3 subtract(Vec3 a,Vec3 b){return {Scalar::sub(a.x,b.x),Scalar::sub(a.y,b.y),Scalar::sub(a.z,b.z)};}
float ease(float fraction,u32 mode){
    const auto t=number(fraction),one=number(1);
    if(mode>=1&&mode<=3){auto result=t*t;for(u32 n=1;n<mode;++n)result=result*t;return result.to_float();}
    if(mode>=4&&mode<=6){const auto inverse=one-t,rounded=number(inverse.to_float());auto result=inverse*rounded;for(u32 n=4;n<mode;++n)result=result*rounded;return (one-number(result.to_float())).to_float();}
    return fraction;
}
}
// Original movement instruction bodies in 0x4184b0 and helper functions
// 0x420d10, 0x420f40, 0x422020, 0x4222b0, 0x4224a0, 0x42c180.
bool EclVm::execute_motion(const EclInstruction& instruction){
    const u32 counts[]={2,4,2,4,3,2,4,1,1,7,4,3,4,0};
    const i32 op=instruction.opcode;const u32 count=op==178?3:op>=63&&op<=76?counts[op-63]:0;
    if(instruction.size<12+count*4){invalid=true;return false;}
    const auto f=[&](u32 i){return operand_float(instruction,i);};
    const auto n=[&](u32 i){return operand_int(instruction,i);};
    const auto set_duration=[&](i32 duration){movement_duration=duration;movement_time.set(duration);};
    const auto mode=[&](u32 value){flags=(flags&~0x3000u)|value<<12;};
    const auto aimed=[&](){const Vec3 player=environment?environment->player:Vec3{};const Vec3 delta=subtract(player,position);return delta.x==0&&delta.y==0?half_pi:angle(delta.x,delta.y);};
    const auto setup_velocity=[&](float direction,u32 speed_arg,bool mirror){
        const float sx=f(speed_arg);const i32 tx=n(0);target.x=(cosine(direction)*number(sx)*Extended::from_int(tx)).to_float();
        const float sy=f(speed_arg);const i32 ty=n(0);target.y=(sine(direction)*number(sy)*Extended::from_int(ty)).to_float();target.z=0;
        origin=resolved_position;set_duration(n(0));flags=(flags&~0x1c000u)|(u32(n(1))&7)<<14;mode(2);
        if(mirror&&(flags&0x40000))target.x=-target.x;
    };
    switch(op){
    case 63:
        position.x=f(0);position.y=f(1);position.z=0;
        clamp_position();
        break;
    case 64:{const Vec3 destination{f(2),f(3),0};target=subtract(destination,resolved_position);origin=position;set_duration(n(0));flags=(flags&~0x1c000u)|(u32(n(1))&7)<<14;mode(2);velocity={};if(flags&0x40000)target.x=-target.x;break;}
    case 65:direction.z=add_angle(f(0),0);speed=f(1);mode(1);set_duration(0);break;
    case 66:case 69:
        if(n(0)<1){const float offset=f(2);direction.z=add_angle(offset,op==69?aimed():0.f);speed=f(3);mode(1);set_duration(op==69?n(0):0);}
        else setup_velocity(add_angle(f(2),0),3,true);
        break;
    case 68:{const float offset=f(0);direction.z=add_angle(offset,aimed());speed=f(1);break;}
    case 70:angular_velocity=f(0);mode(1);break;
    case 71:acceleration=f(0);mode(1);break;
    case 72:set_duration(n(0));origin.x=f(1);origin.y=f(2);orbit_angle=f(3);orbit_velocity=f(4);orbit_radius=f(5);orbit_growth=f(6);mode(3);break;
    case 73:set_duration(n(0));origin=position;orbit_angle=f(1);orbit_velocity=f(2);orbit_radius=0;orbit_growth=f(3);mode(3);break;
    case 74:set_duration(n(0));orbit_velocity=f(1);orbit_growth=f(2);mode(3);break;
    case 75:for(u32 i=0;i<4;++i)bounds[i]=f(i);flags|=0x80000;break;
    case 76:flags&=~0x80000u;break;
    case 67:case 178:{
        if(!random){invalid=true;return false;}const float px=environment?environment->player.x:0;float direction;
        if(op==178){
            if(random->bounded32(4)==0)direction=random->signed_range(pi).to_float();
            else if(position.x<=px){
                if(less_equal(number(position.x)-(number(px)-number(384)),number(px)-number(position.x)))direction=add_angle((random->range(half_pi)+number(2.356194496154785f)).to_float(),0);
                else direction=(random->range(half_pi)-number(.7853981852531433f)).to_float();
            }else if(less_equal((number(px)+number(384))-number(position.x),number(position.x)-number(px)))direction=add_angle((random->range(half_pi)-number(.7853981852531433f)).to_float(),0);
            else direction=add_angle((random->range(half_pi)+number(2.356194496154785f)).to_float(),0);
        }else{
            direction=position.x<=px?(random->range(half_pi)-number(.7853981852531433f)).to_float():add_angle((random->range(half_pi)+number(2.356194496154785f)).to_float(),0);
            if(number(position.x)<number(bounds[0])+number(96)){
                if(direction>half_pi)direction=Scalar::sub(pi,direction);else if(direction<-half_pi)direction=(-number(pi)-number(direction)).to_float();
            }
            if(number(bounds[2])-number(96)<number(position.x)){
                if(direction>=half_pi||direction<0){if(direction>-half_pi&&direction<=0)direction=(-number(pi)-number(direction)).to_float();}
                else direction=Scalar::sub(pi,this->direction.z);
            }
        }
        if(number(position.y)<number(bounds[1])+number(48)&&direction<0)direction=-direction;
        if(number(bounds[3])-number(48)<number(position.y)&&direction>0)direction=-direction;
        if(n(0)<1){this->direction.z=direction;speed=f(2);mode(1);set_duration(0);}
        else setup_velocity(direction,2,false);
        break;
    }
    default:return false;
    }
    return true;
}
// The original ECL tick invokes 0x422c40 after its timers and interpolation
// callbacks. Applying the resulting displacement is the enemy manager's job.
void EclVm::update_motion(const FrameTiming& timing){
    const u32 mode=(flags>>12)&3;
    if(mode==1){
        direction.z=add_angle(direction.z,Scalar::mul(timing.rate,angular_velocity));
        speed=(number(timing.rate)*number(acceleration)+number(speed)).to_float();
        velocity={(cosine(direction.z)*number(speed)).to_float(),(sine(direction.z)*number(speed)).to_float(),0};
    }else if(mode==2){
        movement_time.decrement(1,timing);
        float fraction=(number(1)-movement_time.value()/Extended::from_int(movement_duration)).to_float();if(fraction<0)fraction=0;
        fraction=ease(fraction,(flags>>14)&7);
        const Vec3 delta{Scalar::mul(target.x,fraction),Scalar::mul(target.y,fraction),Scalar::mul(target.z,fraction)};
        velocity=subtract(add(origin,delta),position);if(flags&0x40000)velocity.x=-velocity.x;
        direction.z=angle(velocity.x,velocity.y);
        if(movement_time.current<=0){flags&=~0x3000u;position=add(origin,target);velocity={};}
        return;
    }else if(mode==3){
        orbit_angle=add_angle(orbit_angle,Scalar::mul(timing.rate,orbit_velocity));
        orbit_radius=(number(timing.rate)*number(orbit_growth)+number(orbit_radius)).to_float();
        const float x=(cosine(orbit_angle)*number(orbit_radius)).to_float(),y=(sine(orbit_angle)*number(orbit_radius)).to_float();
        velocity.x=(number(x)+number(origin.x)-number(position.x)).to_float();velocity.y=(number(y)+number(origin.y)-number(position.y)).to_float();
        direction.z=angle(velocity.x,velocity.y);
    }
    if((mode==1||mode==3)&&movement_duration>0){movement_time.decrement(1,timing);if(movement_time.current<=0)flags&=~0x3000u;}
}
}
