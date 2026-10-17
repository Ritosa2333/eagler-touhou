#include "CurveProgram.hpp"
#include <cmath>
namespace th15 {
bool CurveProgram::append_transition(CurveLaserMotion& motion,const BulletTransform& entry){
    if(motion.path.segments.empty()){error="Curve transform requires an initial path";return false;}if(motion.path.segments.size()>100000){error="Curve path exceeded segment limit";return false;}
    const float start=float(entry.integers[1]);auto& previous=motion.path.segments.back();previous.end=start;CurvePathSample sample;if(!CurvePath::evaluate(previous,start,sample)){error="Unsupported previous curve path mode";return false;}
    const auto make=[&](float begin,const CurvePathSample& point){CurvePathSegment s;s.begin=begin;s.direction={float(std::cos(double(point.angle))),float(std::sin(double(point.angle))),0};s.origin={point.position.x,point.position.y,0};s.angle=point.angle;s.speed=point.speed;return s;};
    auto segment=make(start,sample);segment.mode=entry.type==4?CurvePathMode::acceleration:CurvePathMode::angular;segment.acceleration=entry.floats[0];segment.angular_acceleration=entry.floats[1];if(entry.integers[0]>=0)segment.end=float(float(entry.integers[0])+start);motion.path.segments.push_back(segment);
    if(entry.integers[0]>=0){if(!CurvePath::evaluate(segment,segment.end,sample)){error="Unsupported curve transition";return false;}motion.path.segments.push_back(make(segment.end,sample));}return true;
}
bool CurveProgram::activate(std::array<BulletTransform,18>& entries,CurveProgramContext& context){
    for(u32 steps=0;steps<100000;steps++){if(index>=18)return true;if(index<0){error="Negative curve transform index";return false;}auto& entry=entries[u32(index)];if(!entry.type||(!entry.active&&flags)||(entry.type&flags))return true;auto& motion=context.motion;
        switch(entry.type){
        case 1:flags|=1;boost_timer.set(0);boost_stage=0;break;
        case 4:case 8:if(!append_transition(motion,entry))return false;break;
        case 16:flags|=16;turn.angle=entry.floats[0];turn.speed=entry.floats[1]<=-999.f?motion.speed:entry.floats[1];turn.timer.set(0);turn.duration=entry.integers[0];turn.limit=entry.integers[1];turn.count=0;turn.mode=entry.integers[2];break;
        case 64:if(entry.integers[0]>0){flags|=64;reflection.speed=entry.floats[0]<0?motion.speed:entry.floats[0];entry.integers[0]=wrapping_sub(entry.integers[0],1);reflection.limit=entry.integers[0];reflection.count=0;reflection.sides=u32(entry.integers[1]);}break;
        case 128:context.protection=entry.integers[0];break;
        case 256:flags|=256;protection_timer.set(entry.integers[0]);protection_parameter=entry.integers[1];break;
        case 512:if(!context.host.rebind_curve_body(entry.integers[0],entry.integers[1])){error="Curve animation replacement failed";return false;}break;
        case 1024:context.state=3;break;
        case 2048:context.host.laser_sound(entry.integers[0],false);break;
        case 4096:flags|=4096;wait.set(entry.integers[0]);break;
        case 8192:{if(index+1>=18){error="Truncated curve bullet emission";return false;}const auto& next=entries[u32(index+1)];const u32 packed=u32(entry.integers[0]);BulletShooter shooter;shooter.position={float(motion.position.x+float(std::cos(double(motion.angle))*double(motion.emission_distance))),float(motion.position.y+float(std::sin(double(motion.angle))*double(motion.emission_distance))),0};shooter.sprite=i32((packed>>16)&255);shooter.color=i32((packed>>8)&255);shooter.pattern=i16((packed>>24)&127);shooter.count=i16(entry.integers[1]);shooter.rows=i16(next.integers[0]);shooter.speed=entry.floats[0];shooter.speed_step=entry.floats[1];shooter.angle=next.floats[0];shooter.angle_step=next.floats[1];shooter.flags=u32(next.integers[1]);shooter.transform_sound=-1;shooter.tail={packed&255,0};shooter.transforms=entries;index=wrapping_add(index,1);if(!context.host.emit_laser_bullets(shooter)){error="Curve bullet emitter unavailable";return false;}index=wrapping_add(index,1);if(packed&0x80000000u){if(!context.host.erase_emitting_laser()){error="Curve emission cancellation failed";return false;}index=wrapping_add(index,1);}continue;}
        case 32768:context.id=u32(entry.integers[0]);break;
        case 65536:index=entry.integers[0];continue;
        case 1048576:context.host.curve_blend(entry.integers[0]!=0);break;
        case 0x80000000u:if(entry.integers[0]>0){flags|=0x80000000u;freeze.set(entry.integers[0]);}break;
        // The native curve dispatch explicitly advances over other types.
        default:break;
        }index=wrapping_add(index,1);
    }error="Curve transform program exceeded instruction limit";return false;
}
bool CurveProgram::update(std::array<BulletTransform,18>& entries,CurveProgramContext& context,float rate){
    struct Queued:BulletSoundHost {CurveProgramHost& host;explicit Queued(CurveProgramHost& h):host(h){}void play(i32 id)override{host.laser_sound(id,true);}} queued(context.host);
    for(u32 steps=0;steps<100000;steps++){if(!activate(entries,context))return false;if(!flags)return true;u32 completed=0;MovingLaserMotion kinematics;kinematics.position=context.motion.position;kinematics.velocity=context.motion.velocity;kinematics.angle=context.motion.angle;kinematics.speed=context.motion.speed;
        if(flags&4)completed+=acceleration.advance(kinematics,flags,rate);
        if(flags&8){if(angular_timer.current>=angular_duration){flags&=~8u;completed++;}else{kinematics.angle=normalize_angle(float(float(angular_angle*rate)+kinematics.angle));kinematics.speed=float(float(angular_speed*rate)+kinematics.speed);kinematics.set_velocity();angular_timer.tick(&rate);}}
        if((flags&16)&&turn.mode==0)completed+=turn.advance(kinematics,flags,rate,context.queued_sound,&queued);context.motion.velocity=kinematics.velocity;context.motion.angle=kinematics.angle;context.motion.speed=kinematics.speed;
        if(flags&256){protection_timer.decrement(&rate);if(protection_timer.current<1){flags^=256;completed++;}}
        if(flags&0x80000000u){if(freeze.current<=0){flags^=0x80000000u;completed++;}else freeze.decrement(&rate);}
        // Native boost, reflection, wait and other turn modes have no step.
        if(context.protection)context.protection=wrapping_sub(context.protection,1);if(!completed)return true;
    }error="Curve transform dispatch exceeded instruction limit";return false;
}
}
