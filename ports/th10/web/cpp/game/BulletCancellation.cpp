#include "BulletCancellation.hpp"
#include "PlayerShooting.hpp"
#include "BulletFrame.hpp"
namespace th10 {
// Pool loops in 0x425730. Cancellation can change a laser's linked-list fields;
// retain its successor before calling the object-specific cancellation method.
void cancel_bullets(EnemyBullet* pool,bool include_protected,BulletCancellationEnvironment& env){
    for(u32 i=0;i<2000;++i){auto& bullet=pool[i];if(bullet.state&&bullet.state!=3&&(include_protected||!bullet.cancel_protection))env.cancel_bullet(bullet);}
}
// 0x41c850 and its inlined variant. State 1 is excluded in both modes.
void cancel_lasers(EnemyLaser* head,bool convert_items,BulletCancellationEnvironment& env){
    while(head){auto* next=head->next;if(head->state!=1)env.cancel_laser(*head,convert_items);head=next;}
}
// 0x408030: cancellation and offscreen removal have different timer behavior.
i32 EnemyBullet::cancel(BulletEffectEnvironment& env){
    if(state!=1&&state!=2)return 0;
    const bool outside=outside_playfield(motion.position,8.f,8.f);animation.pending_interrupt=1;state=3;
    if(outside)flags|=8;
    else{
        if(cancel_script>=0)env.manager->create_at(*env.effect_file,cancel_script,motion.position,true,AnimationPlacement::WorldBack,*env.animations,*env.allocation);
        if(!(cancel_timer_flags&1)){cancel_timer_flags|=1;cancel_timer.rate=env.default_rate;}cancel_timer.initialize(-1);
    }
    return 1;
}
// 0x408100: the original spills the displacement, but retains the radius sum in
// extended precision. Unordered comparisons take the cancellation branch.
i32 cancel_bullet_circle(EnemyBullet* pool,const Vec3& position,float radius,bool convert_items,bool respect_protection,BulletEffectEnvironment& env){
    for(u32 i=0;i<2000;++i){
        auto& bullet=pool[i];if(!bullet.state||bullet.state==3||(respect_protection&&bullet.cancel_protection))continue;
        const float dx=Scalar::sub(bullet.motion.position.x,position.x),dy=Scalar::sub(bullet.motion.position.y,position.y);
        const auto reach=number(bullet.cancel_size)*number(.5f)+number(radius),distance=number(dy)*number(dy)+number(dx)*number(dx);
        if(reach*reach<distance)continue;
        bullet.cancel(env);if(!outside_playfield(bullet.motion.position,2.f,2.f)&&convert_items)env.spawn_faith(bullet.motion.position);
    }
    return 0;
}
// 0x408210. This rectangular clear flags bullets for removal and creates its
// own colored effect; it does not use the ordinary cancellation state machine.
i32 EnemyBulletManager::cancel_rectangle(i32 convert,const u32* small,const u32* medium,const u32* large,BulletEffectEnvironment& env){
    const float half_x=Scalar::mul(last_cancel_size.x,.5f),half_y=Scalar::mul(last_cancel_size.y,.5f);
    const float left=Scalar::sub(last_cancel_position.x,half_x),right=Scalar::add(half_x,last_cancel_position.x);
    const float top=Scalar::sub(last_cancel_position.y,half_y),bottom=Scalar::add(half_y,last_cancel_position.y);
    for(u32 i=0;i<2000;++i){auto& bullet=pool[i];if(!bullet.state||bullet.state==3)continue;
        const auto half_width=number(bullet.cancel_size)*number(.5f);const float half_height=Scalar::mul(bullet.hitbox_height,.5f);
        const float bullet_left=(number(bullet.motion.position.x)-half_width).to_float(),bullet_top=Scalar::sub(bullet.motion.position.y,half_height);
        const auto bullet_right=half_width+number(bullet.motion.position.x);const float bullet_bottom=Scalar::add(half_height,bullet.motion.position.y);
        if(bullet_right<number(left)||number(right)<number(bullet_left)||number(bullet_bottom)<number(top)||number(bottom)<number(bullet_top))continue;
        bullet.flags|=8;if(convert)env.spawn_faith(bullet.motion.position);
        const u32 id=env.manager->create_at(*animation_file,0x173,bullet.motion.position,true,AnimationPlacement::WorldBack,*env.animations,*env.allocation);
        auto* effect=env.manager->registry.find(id);
        if(bullet.animation.sprite){const float width=bullet.animation.sprite->width;effect->color=(width<=16.f?small:width<=32.f?medium:large)[bullet.color];}
        bullet.reserved_450=0;
    }
    return 0;
}
// 0x408460.
void EnemyBulletManager::cancel_all(bool respect_protection,BulletEffectEnvironment& env){
    for(u32 i=0;i<2000;++i){auto& bullet=pool[i];if(bullet.state&&bullet.state!=3&&(!respect_protection||!bullet.cancel_protection))bullet.cancel(env);}
}
// 0x4084a0. The original query counts rectangle corners, with weights based on
// sprite width. A circle wholly inside a large rectangle need not count it.
i32 EnemyBulletManager::count_in_circle(const Vec3& point,float radius) const noexcept {
    const auto squared=number(radius)*number(radius);i32 count=0;
    for(u32 i=0;i<2000;++i){const auto& bullet=pool[i];if(!bullet.state||bullet.state==3)continue;
        const auto half_width=number(bullet.cancel_size)*number(.5f),half_height=number(bullet.hitbox_height)*number(.5f);
        const auto left=number(bullet.motion.position.x)-half_width,top=number(bullet.motion.position.y)-half_height;
        const float left_top=top.to_float(),right=(half_width+number(bullet.motion.position.x)).to_float(),bottom=(half_height+number(bullet.motion.position.y)).to_float();
        const float left_bottom=(top+number(bullet.hitbox_height)).to_float(),right_top=Scalar::sub(bottom,bullet.hitbox_height);
        const auto dx=number(point.x)-left,dx2=dx*dx;const float dy=Scalar::sub(point.y,left_top);
        bool hit=!(squared<number(dy)*number(dy)+dx2);
        if(!hit){const auto delta=number(point.y)-number(left_bottom);hit=!(squared<delta*delta+dx2);}
        if(!hit){const auto delta=number(point.x)-number(right);const float horizontal=(delta*delta).to_float();auto vertical=number(point.y)-number(bottom);hit=!(squared<vertical*vertical+number(horizontal));
            if(!hit){vertical=number(point.y)-number(right_top);hit=!(squared<vertical*vertical+number(horizontal));}}
        if(!hit||!bullet.animation.sprite)continue;
        const float width=bullet.animation.sprite->width;if(width<=16.f)count=wrapping_add(count,1);else if(width<=32.f)count=wrapping_add(count,4);else if(width<=64.f)count=wrapping_add(count,10);
    }
    return count;
}
}
