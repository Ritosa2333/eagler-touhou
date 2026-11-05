#include "BulletState.hpp"
#include <algorithm>
namespace th15 {
PlayerContact BulletState::collide(const PlayerCollision& player,bool graze_only,float rate,Rng& random,Rng& visual_random,BulletContactHost& host,std::string& error){
    visual_flags&=0xfff9ffffu;visual_jitter={};if(!(flags&2))return PlayerContact::none;
    Vec2 size=hitbox;if(flags&64){size.x=float(size.x*scale);size.y=float(size.y*scale);}
    const Vec2 position{motion.position.x,motion.position.y};
    const auto contact=(flags&16)?player.circle(position,size.x,graze_only,&host):player.rectangle(position,size,graze_only,&host);
    if(contact==PlayerContact::hit&&collision_delay==0){
        phase=3;if(!host.hit_animation()){error="Bullet hit animation callback unavailable";return contact;}hit_interrupt=1;
        if(overlay_active&&!host.overlay_interrupt(1)){error="Bullet overlay interrupt unavailable";return contact;}
        if(cancellation_script>=0)host.cancellation_effect(cancellation_script,motion.position,{float(float(motion.velocity.x*rate)*10.f),float(float(motion.velocity.y*rate)*10.f),float(float(motion.velocity.z*rate)*10.f)});
    }else if(contact==PlayerContact::graze){
        if(graze_flash.current!=graze_flash.previous){
            if(!graze_interval||(graze_flash.current==INT32_MIN&&graze_interval==-1)){error="Invalid bullet graze interval";return contact;}
            if(graze_flash.current%graze_interval==0){host.graze_spark(motion.position);graze_flash.set(0);}
        }
        graze_flash.tick(&rate);graze_duration.tick(&rate);host.graze();host.graze_resonance(.3f);
        if(graze_duration.current<45){
            const i32 channel=std::min(wrapping_mul(wrapping_sub(104,graze_duration.current),2),240);
            visual_flags=(visual_flags&0xfffbffffu)|0x20000u;graze_color=0xffff0080u|(u32(u8(channel))<<8);
            const float y=visual_random.signed_unit(),x=visual_random.signed_unit();visual_jitter={x,y,0};
        }else if(graze_duration.current==45){
            graze_interval=30;host.play(76);const float angle=float(float(random.signed_unit()*.1745329201221466064453125f)-1.57079637050628662109375f);host.item(13,motion.position,angle,2.2f);
        }
    }
    return contact;
}
}
