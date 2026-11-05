#include "BulletFrame.hpp"
#include "GameMath.hpp"
#include "HighRefresh.hpp"
namespace th10 {
namespace {
void reset_timer(Timer& timer,u32& flags,const float* rate){if(!(flags&1)){flags|=1;timer.rate=rate;}timer.initialize(-1);}
void move(EnemyBulletMotion& motion,float rate,bool half){
    auto x=number(rate)*number(motion.velocity.x),y=number(rate)*number(motion.velocity.y);
    auto z=number(Scalar::mul(rate,motion.velocity.z));
    if(half){x=number((x*number(.5f)).to_float());y=y*number(.5f);z=z*number(.5f);}
    motion.position.x=(x+number(motion.position.x)).to_float();motion.position.y=(y+number(motion.position.y)).to_float();motion.position.z=(z+number(motion.position.z)).to_float();
}
void effect(EnemyBullet& bullet,i32 script,BulletFrameEnvironment& env){env.manager->create_at(*env.effect_file,script,bullet.motion.position,true,AnimationPlacement::WorldBack,*env.animations,*env.allocation);}
}
// 0x405d00 / 0x405de0. Construction preserves non-animation storage.
void EnemyBullet::initialize() noexcept {
    animation.clear();cancel_timer_flags&=~1u;secondary_timer_flags&=~1u;for(auto& modifier:modifiers)modifier.timer_flags&=~1u;
}
void EnemyBullet::release(AnmAllocationEnvironment& env){if(animation.geometry)env.release_memory(animation.geometry);animation.geometry=nullptr;}
// 0x405be0. Recycling only resets state and the two phase counters.
void EnemyBullet::reset(const float* rate) noexcept {state=0;reset_timer(cancel_timer,cancel_timer_flags,rate);reset_timer(secondary_timer,secondary_timer_flags,rate);}
// 0x406160 / 0x4061d0, with/without the extra 64 pixels above the playfield.
bool bullet_outside_playfield(const Vec3& point,float width,float height,bool extended_top) noexcept {
    const auto half_x=number(width)*number(.5f),half_y=number(height)*number(.5f);
    const auto left=half_x+number(point.x),right=number(point.x)-half_x,top=half_y+number(point.y),bottom=number(point.y)-half_y;
    const auto top_edge=number(extended_top?-64.f:0.f);
    return left<number(-192.f)||left==number(-192.f)||number(192.f)<right||right==number(192.f)||top<top_edge||top==top_edge||number(448.f)<bottom||bottom==number(448.f);
}
// 0x406240. Spawn animations move at half speed; finishing that animation
// switches to the normal path during the same frame, including its movement.
i32 EnemyBullet::update(BulletFrameEnvironment& env){
    if(flags&8){reset(env.default_rate);return -1;}
    bool active=state==1;
    if(state==2){move(motion,*env.default_rate,true);if(animation.integer_variables[0]){state=1;active=true;}}
    else if(state==3)move(motion,*env.default_rate,true);
    if(active){
        env.run_commands(*this);
        if(active_features){
            constexpr u32 masks[]={1,0x10,0x20,0x40,0x100,0x80,0x8000c00,0x4000000};
            for(u32 i=0;i<8;i++)if(active_features&masks[i])env.update_feature(*this,static_cast<BulletFeature>(i));
            if(active_features&0x8000){if(modifiers[5].timer.current<=0)active_features^=0x8000;else modifiers[5].timer.advance(-1.f);}
        }
        move(motion,*env.default_rate,false);
        if(flags&2){
            const i32 collision=env.collide_player(motion.position,{cancel_size,hitbox_height});
            if(collision==1){state=3;animation.pending_interrupt=1;if(cancel_script>=0)effect(*this,cancel_script,env);}
            else if(collision==2&&!(flags&4)){flags|=4;effect(*this,0x1b2,env);env.play_sound(28,motion.position.x);}
        }
    }
    if(animation.sprite){
        if(active_features&0x100000)env.update_feature(*this,BulletFeature::HorizontalWrap);
        if(active_features&0x200000)env.update_feature(*this,BulletFeature::VerticalWrap);
        if(outside_delay<=0&&bullet_outside_playfield(motion.position,animation.sprite->width,animation.sprite->height,true)){reset(env.default_rate);return -1;}
    }
    if(cancel_protection)--cancel_protection;if(outside_delay>0)--outside_delay;
    if(animation.update(*env.animations)){reset(env.default_rate);return -1;}return 0;
}
// 0x4065c0. Rebuild six drawing lists in pool order and advance surviving
// bullets' phase timers, including the special update-suppression mode.
i32 EnemyBulletManager::update(BulletFrameEnvironment& env){
    active_count=0;for(auto& head:draw_heads)head=nullptr;for(auto& tail:draw_tails)tail=nullptr;
    for(u32 i=0;i<2000;i++){
        auto& bullet=pool[i];if(!bullet.state)continue;
        if(!(env.controller_flags&&(*env.controller_flags&0x402)==0x402)&&bullet.update(env)!=0)continue;
        auto*& head=draw_heads[bullet.draw_layer];auto*& tail=draw_tails[bullet.draw_layer];
        if(head)tail->draw_next=&bullet;else head=&bullet;tail=&bullet;bullet.draw_next=nullptr;++active_count;bullet.cancel_timer.tick();
    }
    return 1;
}
// 0x4066e0 / 0x4066c0.
i32 EnemyBulletManager::draw_layer(i32 layer,BulletFrameEnvironment& env){
    for(auto* bullet=draw_heads[layer];bullet;bullet=bullet->draw_next){
        auto& source=bullet->animation;Vec3 point=bullet->motion.position;float angle=bullet->motion.angle;AnmVm copy;AnmVm* vm=&source;
        if(high_refresh::render_only){copy=source;vm=&copy;}env.presentation(*bullet,point,angle);
        vm->script_position={Scalar::add(point.x,224.f),Scalar::add(point.y,16.f),point.z};
        if(vm->flags&0x8000000){vm->rotation.z=add_angle(angle,1.57079637050628662109375f).to_float();vm->flags|=4;}
        env.submit(*vm);
    }
    return 1;
}
i32 EnemyBulletManager::draw(BulletFrameEnvironment& env){for(i32 layer=0;layer<6;layer++)draw_layer(layer,env);return 1;}
i32 EnemyBulletManager::tick(BulletFrameEnvironment& env){return env.controller_flags&&(*env.controller_flags&5)?1:update(env);}
i32 EnemyBulletManager::render(BulletFrameEnvironment& env){return env.controller_flags&&(*env.controller_flags&4)?1:draw(env);}
}
