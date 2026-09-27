#include "BulletUpdate.hpp"
#ifdef TH_MULTIPLAYER_FIXTURES
#include <emscripten.h>
#include <array>
#endif
namespace th08 {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
namespace {bool filter_bullet_target=true;}
#ifdef TH_MULTIPLAYER_FIXTURES
bool fixture_bullet_target_filter(bool enabled) noexcept {filter_bullet_target=enabled;return filter_bullet_target;}
#endif
#endif
#ifdef TH_MULTIPLAYER_FIXTURES
namespace {std::array<double,8> update_profile{};}
const double* fixture_bullet_update_profile() noexcept {return update_profile.data();}
#endif
namespace {
void retire(BulletState& b){b.state=0;b.since_fired.set(0);b.active_time.set(0);}
void move(Vec3& position,const Vec3& velocity,float divisor=1){
    if(divisor==1){position.x=Scalar::add(position.x,velocity.x);position.y=Scalar::add(position.y,velocity.y);position.z=Scalar::add(position.z,velocity.z);}
    else{const float inverse=Scalar::div(1,divisor);position.x=(number(position.x)+number(Scalar::mul(inverse,velocity.x))).to_float();position.y=(number(position.y)+number(Scalar::mul(inverse,velocity.y))).to_float();position.z=(number(position.z)+number(Scalar::mul(inverse,velocity.z))).to_float();}
}
}
void BulletUpdate::cancel_reward(BulletState& b){
    if(!actions)return;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const i32 type=i32(b.padding_dbf)-1;
#else
    const i32 type=live_cancel_item?*live_cancel_item:cancel_item;
#endif
    if(type==9){actions->item(b.position,7);actions->item(b.position,7);}
    else if(type>=0)actions->item(b.position,type);
}
bool BulletUpdate::normal(BulletState& b,BulletMotion& motion){
#ifdef TH_MULTIPLAYER_FIXTURES
    const bool sample=(std::size_t(&b-state.bullets)%32)==0;
    double stamp=sample?emscripten_get_now():0;
    const auto mark=[&](u32 part){if(sample){const double now=emscripten_get_now();update_profile[part]+=now-stamp;stamp=now;}};
    if(sample)++update_profile[5];
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Only extra flag 0x80 aims at a player. Keep the pre-extra position:
    // initialize_extra may activate 0x80 during this tick.
    const Vec3 target_origin=b.position;
#ifdef TH_MULTIPLAYER_FIXTURES
    if(!filter_bullet_target&&actions)motion.player=actions->target(target_origin);
#endif
#endif
    if(!creation.initialize_extra(b))return false;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(filter_bullet_target&&actions&&(b.extra_flags&0x80u))motion.player=actions->target(target_origin);
#endif
#ifdef TH_MULTIPLAYER_FIXTURES
    mark(0);
#endif
    motion.update(b);
#ifdef TH_MULTIPLAYER_FIXTURES
    mark(1);
#endif
    if(b.despawn_protection)b.despawn_protection=wrapping_sub(b.despawn_protection,1);
    if(!paused)move(b.position,b.velocity);
    if(!b.despawn_protection){
        const auto* sprite=b.sprites.animation[0].loadedSprite;if(!sprite)return false;
        if(bullet_in_view(b.position,sprite->widthPx,sprite->heightPx))b.despawn_counter=0;
        else if(b.extra_flags&0xdc0){++b.despawn_counter;if(b.despawn_counter>=128){retire(b);return true;}}
        else if(b.despawn_counter)--b.despawn_counter;else{retire(b);return true;}
    }
#ifdef TH_MULTIPLAYER_FIXTURES
    mark(2);
#endif
    bool test_hit=true;
    if(!b.reisen_illusion){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if(b.active_time.current>=16){
            // Per-seat graze ownership is a bitmask in multiplayer. Once a
            // seat has grazed this bullet, the graze pass deliberately skips
            // that seat on later frames. A zero aggregate graze result
            // therefore does NOT mean the bullet is too far away to hit;
            // suppressing the hit pass here made already-grazed bullets pass
            // straight through player hitboxes. Only a barrier/cancel result
            // may stop the subsequent lethal collision test.
            const i32 result=collision(1,b);
            if(result==2&&!(b.flags&0x1000)){b.state=5;cancel_reward(b);}
            if(result==2)test_hit=false;
        }
#else
        if(!b.grazed&&b.active_time.current>=16){
            const i32 result=collision(1,b);
            if(result==1)b.grazed=1;
            else
            {if(result==2&&!(b.flags&0x1000)){b.state=5;cancel_reward(b);}if(result!=1)test_hit=false;}
        }
#endif
        if(test_hit){const i32 result=collision(2,b);if(result!=0&&(result!=2||!(b.flags&0x1000))){b.state=5;if(result==2)cancel_reward(b);}}
    }
#ifdef TH_MULTIPLAYER_FIXTURES
    mark(3);
#endif
    if(b.sprites.animation[0].currentInstruction)animation.execute(b.sprites.animation[0]);
#ifdef TH_MULTIPLAYER_FIXTURES
    mark(4);
#endif
    return !animation.invalid;
}
bool BulletUpdate::update_bullets(){
    creation.timing=timing;animation.timing=timing;animation.invalid=false;
    BulletMotion motion;motion.timing=timing;motion.player=player;motion.actions=creation.actions;
    state.active_count=0;for(auto& layer:state.layers)layer=nullptr;
    // The original processes slot zero first, then 1535 down to 1.
    for(u32 ordinal=0;ordinal<1536;++ordinal){
        auto& b=state.bullets[ordinal?1536-ordinal:0];if(!b.state)continue;++state.active_count;
        const u16 starting=b.state;
        if(starting>=2&&starting<=4){
            b.active_time.decrement(1,timing);move(b.position,b.velocity,starting==2?2:starting==3?2.5f:3);
            if(!(b.flags&0x1000)&&collision(0,b)==2)b.unknown_dbe=1;
            if(animation.execute(b.sprites.animation[starting-1])){
                if(b.unknown_dbe){b.state=5;cancel_reward(b);}
                // Original unconditionally enters normal state after the reward.
                b.state=1;b.since_fired.set(0);if(!normal(b,motion))return false;
            }
        }else if(starting==1){if(!normal(b,motion))return false;}
        else if(starting==5){move(b.position,b.velocity,2);if(animation.execute(b.sprites.animation[4]))retire(b);}
        if(animation.invalid)return false;if(!b.state)continue;
        b.since_fired.tick(timing);b.active_time.tick(timing);
        if(b.sprites.layer>=6)return false;b.next_in_layer=state.layers[b.sprites.layer];state.layers[b.sprites.layer]=&b;
    }
    return true;
}
}
