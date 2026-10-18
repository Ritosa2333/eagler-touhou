#include "BulletState.hpp"
namespace th15 {
bool BulletState::appearance(i32 type,i32 new_color,BulletAnimationHost& host,bool replacement,std::string& error){
    const auto* p=bullet_appearance(type);const i32 index=replacement?(u16(new_color)&0x7fff):new_color;
    if(!p||index<0||index>=16||(p->cancel_kind==1&&index>=8)){error="Invalid bullet appearance specification";return false;}
    sprite_type=i16(type);color=i16(index);hitbox={p->radius,p->radius};cancel_item=p->item;
    visual_flags=7;hit_interrupt=0;visual_jitter={};graze_color=0;spawn_animation_finished=false;
    if(!host.bind_appearance(p->script,p->overlay)){error="Bullet appearance resource binding failed";return false;}
    flags|=16;overlay_active=p->overlay!=0;cancellation_script=bullet_cancellation(*p,index,cancellation_script,replacement);
    if(replacement&&(new_color&0x8000)){if(!host.spawn_animation()){error="Bullet replacement spawn callback unavailable";return false;}hit_interrupt=2;}
    return true;
}
bool BulletState::initialize(const BulletShooter& shooter,i32 column,i32 row,float aim,const Vec3& player,float minimum_distance_squared,Rng& random,BulletFrameHost& host,std::string& error){
    const bool accepted=form_bullet(shooter,column,row,aim,random,player,minimum_distance_squared,motion);
    flags|=1;phase=1;spawn.set(0);lifetime.set(0);collision_delay=0;scale=1;scale_curve.duration=0;
    if(!accepted){host.recycle();return false;}
    flags=(flags&0xfffffff3u)|2u;graze_interval=60;graze_flash.set(0);graze_duration.set(0);
    if(!appearance(shooter.sprite,shooter.color,host,false,error))return false;
    transform_sound=shooter.transform_sound;offscreen_grace=5;initial_transform_flags=shooter.flags;transform_flags=0;transform_index=i32(shooter.tail[0]);transforms=shooter.transforms;
    if(transform_index<0||transform_index>=18){error="Invalid initial bullet transform cursor";return false;}
    const auto& first=transforms[transform_index];
    if(first.type==2){
        if(i16(first.integers[0])!=1&&!host.interrupt(wrapping_add(i32(i16(first.integers[0])),7))){error="Bullet initial animation interrupt unavailable";return false;}
        phase=2;motion.position={float(motion.position.x-float(motion.velocity.x*4.f)),float(motion.position.y-float(motion.velocity.y*4.f)),float(motion.position.z-float(motion.velocity.z*4.f))};transform_index=wrapping_add(transform_index,1);
    }else{if(!host.spawn_animation()){error="Bullet initial spawn callback unavailable";return false;}hit_interrupt=2;}
    if(!activate(player,random,&host,&host,error))return false;
    host.animation_finished(false);if(overlay_active)host.animation_finished(true);return true;
}
}
