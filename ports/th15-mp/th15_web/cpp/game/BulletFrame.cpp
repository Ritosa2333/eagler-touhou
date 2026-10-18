#include "BulletState.hpp"
namespace th15 {
BulletFrameResult BulletState::update(const BulletFrameContext& context,Rng& random,BulletFrameHost& host,std::string& error){
    const float rate=context.rate;lifetime.tick(&rate);
    // Transform steps may replace the selected sprite during this update.
    // Native 0x419390 resolves its current dimensions after those steps.
    auto sprite_size=[&](){Vec2 size=context.sprite_size;host.sprite_dimensions(size);return size;};
    auto recycle=[&](){host.recycle();return BulletFrameResult::recycled;};
    if(flags&8)return recycle();
    if(transform_flags&0x400000){scale=scale_curve.step(rate)[0];if(!scale_curve.duration){transform_flags&=~0x400000u;if(scale==1.f)flags&=~64u;}}
    auto move=[&](float factor){motion.position={float(motion.position.x+float(float(motion.velocity.x*rate)*factor)),float(motion.position.y+float(float(motion.velocity.y*rate)*factor)),float(motion.position.z+float(float(motion.velocity.z*rate)*factor))};};
    bool active=phase==1;
    if(phase==2){move(.5f);if((spawn.current<8||host.contact(*this,false)!=1)&&spawn_animation_finished){phase=1;active=true;}}
    else if(phase==3)move(.5f);
    else if(phase==5&&spawn.current>=3){
        if(spawn.current==3){if(!host.interrupt(1)){error="Bullet cancellation animation unavailable";return BulletFrameResult::error;}
            if(cancellation_script>=0)host.cancellation_effect(cancellation_script,motion.position,{float(float(motion.velocity.x*rate)*10.f),float(float(motion.velocity.y*rate)*10.f),float(float(motion.velocity.z*rate)*10.f)});
        }move(.5f);
    }
    if(active){
        u32 iterations=0;
        for(;;){
            if(++iterations>4096){error="Bullet transform completion cycle";return BulletFrameResult::error;}
            if(!(transform_flags&0x4000000)&&!activate(context.player,random,&host,&host,error))return BulletFrameResult::error;
            if(!transform_flags)break;
            u32 completed=0;
            if(transform_flags&1)completed+=update_boost(rate);
            if(transform_flags&4)completed+=update_acceleration(rate);
            if(transform_flags&0x200000)completed+=update_acceleration(rate,true);
            if(transform_flags&8)completed+=update_angular(rate);
            if(transform_flags&16)completed+=update_turn(rate,context.player,&host);
            if(transform_flags&64)completed+=update_reflection(context.reflection_bounds,&host);
            if(transform_flags&0x20000)completed+=update_position_curve(rate);
            if(transform_flags&0x80000)completed+=update_drift(rate);
            if(transform_flags&0x100)completed+=update_wait(rate,sprite_size());
            if(transform_flags&0x80000000u){if(invulnerability.current<1){transform_flags^=0x80000000u;completed++;}else invulnerability.decrement(&rate);}
            if(transform_flags&0x4000000){if(freeze.current<1){flags&=~0x200u;transform_flags^=0x4000000;completed++;}else{flags|=0x200;freeze.decrement(&rate);}}
            if(collision_delay)collision_delay=wrapping_add(collision_delay,-1);
            if(!completed)break;
        }
        if(!(flags&0x200)){move(1.f);host.contact(*this,false);}
    }
    Vec2 current_size=context.sprite_size;const bool available=host.sprite_dimensions(current_size)||context.sprite_available;
    if(available){
        if(transform_flags&0x1000)update_wrap(current_size,&host);
        if(!(transform_flags&0x100)&&offscreen_grace<1){
            const float half_width=float(float(current_size.x*scale)*.5f),half_height=float(float(current_size.y*scale)*.5f);
            if(float(motion.position.x+half_width)<=-192.f||192.f<=float(motion.position.x-half_width)||float(motion.position.y+half_height)<=-64.f||448.f<=float(motion.position.y-half_height))return recycle();
        }
    }
    if(collision_delay)collision_delay=wrapping_add(collision_delay,-1);
    if(offscreen_grace>0)offscreen_grace=wrapping_add(offscreen_grace,-1);
    if(!(flags&0x200)&&host.animation_finished(false))return recycle();
    if(overlay_active)host.animation_finished(true);
    return BulletFrameResult::alive;
}
}
