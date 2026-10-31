#include "BulletState.hpp"
namespace th17 {
PlayerContact BulletState::collide(const PlayerCollision& player,bool graze_only,float rate,Rng&,Rng&,BulletContactHost& host,std::string& error){
    visual_flags&=0xfff9ffffu;visual_jitter={};if(!(flags&2)||!(hitbox.x>0))return PlayerContact::none;
    Vec2 size=hitbox;if(flags&64){size.x=float(size.x*scale);size.y=float(size.y*scale);}
    const Vec2 position{motion.position.x,motion.position.y};
    const auto contact=(flags&16)?player.circle(position,size.x,graze_only,&host):player.rectangle(position,size,graze_only,&host);
    if(contact==PlayerContact::hit&&collision_delay==0){
        phase=3;if(!host.hit_animation()){error="Bullet hit animation unavailable";return contact;}hit_interrupt=1;
        if(overlay_active&&!host.overlay_interrupt(1)){error="Bullet overlay interrupt unavailable";return contact;}
        if(cancellation_script>=0)host.cancellation_effect(cancellation_script,motion.position,{float(float(motion.velocity.x*rate)*10.f),float(float(motion.velocity.y*rate)*10.f),float(float(motion.velocity.z*rate)*10.f)});
    }else if(contact==PlayerContact::graze&&!(flags&4)){host.graze_spark(motion.position);flags|=4;host.graze();}
    return contact;
}
}
