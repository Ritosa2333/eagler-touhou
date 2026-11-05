#include "LaserProgram.hpp"
#include <cmath>
namespace th15 {
bool LaserProgram::activate(std::array<BulletTransform,18>& program,LaserProgramContext& context){
    for(u32 steps=0;steps<100000;steps++){if(index>=18)return true;if(index<0){error="Negative moving laser transform index";return false;}auto& entry=program[u32(index)];if(!entry.type||(!entry.active&&flags))return true;auto& motion=context.motion;
        switch(entry.type){
        case 1:flags|=1;boost_timer.set(0);boost_stage=0;break;
        case 4:{flags|=4;acceleration.speed=entry.floats[0];float angle=entry.floats[1];if(angle<=-990.f)angle=motion.angle;else if(angle>=990.f)angle=bullet_aim(motion.position,context.player);acceleration.angle=angle;acceleration.timer.set(0);acceleration.duration=entry.integers[0];acceleration.vector={float(std::cos(double(angle))*double(acceleration.speed)),float(std::sin(double(angle))*double(acceleration.speed)),acceleration.vector.z};if(index&&context.queued_sound>=0)context.host.laser_sound(context.queued_sound,true);break;}
        case 8:flags|=8;angular_speed=entry.floats[0];angular_angle=entry.floats[1];angular_timer.set(0);angular_duration=entry.integers[0];if(index&&context.queued_sound>=0)context.host.laser_sound(context.queued_sound,true);break;
        case 16:flags|=16;turn.angle=entry.floats[0];turn.speed=entry.floats[1]<=-999.f?motion.speed:entry.floats[1];turn.timer.set(0);turn.duration=entry.integers[0];turn.limit=entry.integers[1];turn.count=0;turn.mode=entry.integers[2];break;
        case 64:if(entry.integers[0]>0){flags|=64;reflection.speed=entry.floats[0]<0?motion.speed:entry.floats[0];entry.integers[0]=wrapping_sub(entry.integers[0],1);reflection.limit=entry.integers[0];reflection.count=0;reflection.sides=u32(entry.integers[1]);}break;
        case 128:context.protection=entry.integers[0];break;
        case 256:motion.invulnerability.set(entry.integers[0]);break;
        case 512:if(!context.visual.rebind_body(entry.integers[0],entry.integers[1])){error=context.visual.error;return false;}break;
        case 1024:context.state=3;break;
        case 2048:context.host.laser_sound(entry.integers[0],false);break;
        case 4096:flags|=0x1000;wait.set(entry.integers[0]);break;
        case 8192:{
            if(index+1>=18){error="Truncated moving laser bullet emission instruction";return false;}
            const auto& next=program[u32(index+1)];const u32 packed=u32(entry.integers[0]);BulletShooter shooter;
            shooter.position={float(motion.position.x+float(std::cos(double(motion.angle))*double(motion.length))),float(motion.position.y+float(std::sin(double(motion.angle))*double(motion.length))),0};
            shooter.sprite=i32((packed>>16)&255);shooter.color=i32((packed>>8)&255);shooter.pattern=i16((packed>>24)&127);shooter.count=i16(entry.integers[1]);shooter.rows=i16(next.integers[0]);
            shooter.speed=entry.floats[0];shooter.speed_step=entry.floats[1];shooter.angle=next.floats[0];shooter.angle_step=next.floats[1];shooter.flags=u32(next.integers[1]);shooter.transform_sound=-1;shooter.tail={packed&255,0};shooter.transforms=program;
            index=wrapping_add(index,1);if(!context.host.emit_laser_bullets(shooter)){error="Moving laser bullet emitter unavailable";return false;}index=wrapping_add(index,1);
            if(packed&0x80000000u){if(!context.host.erase_emitting_laser()){error="Failed to cancel bullet-emitting laser";return false;}index=wrapping_add(index,1);}continue;
        }
        case 32768:context.id=u32(entry.integers[0]);break;
        case 65536:index=entry.integers[0];continue;
        case 1048576:context.visual.body.visual.flags=entry.integers[0]?(context.visual.body.visual.flags&~0x1c0u)|0x20:(context.visual.body.visual.flags&~0x1e0u);break;
        case 0x80000000u:flags|=0x80000000u;freeze.set(entry.integers[0]);break;
        // The moving-laser dispatcher advances past commands belonging only
        // to bullets or other laser kinds, including the bullet-to-laser entry.
        // Original 442370 branches all unrecognized types to 44294b.
        default:break;
        }index=wrapping_add(index,1);
    }error="Moving laser transform program exceeded instruction limit";return false;
}
bool LaserProgram::update(std::array<BulletTransform,18>& entries,LaserProgramContext& context,float rate){
    struct Queued:BulletSoundHost {LaserProgramHost& host;explicit Queued(LaserProgramHost& h):host(h){}void play(i32 id)override{host.laser_sound(id,true);}} queued(context.host);
    for(u32 steps=0;steps<100000;steps++){if(!activate(entries,context))return false;if(!flags)return true;u32 completed=0;
        if(flags&4)completed+=acceleration.advance(context.motion,flags,rate);if((flags&16)&&turn.mode==0)completed+=turn.advance(context.motion,flags,rate,context.queued_sound,&queued);
        // The original moving-laser vtable intentionally leaves boost, angular,
        // nonzero turn modes and the wait step inactive; their flags persist.
        if(flags&0x40){const i32 result=reflection.advance(context.motion,flags,context.queued_sound,context.host);if(result<0){error=reflection.error;return false;}completed+=u32(result);}
        if(flags&0x80000000u){if(freeze.current<=0){flags^=0x80000000u;completed++;}else freeze.decrement(&rate);}
        if(context.protection)context.protection=wrapping_sub(context.protection,1);if(!completed)return true;
    }error="Moving laser transform dispatch exceeded instruction limit";return false;
}
}
