#include "BulletManager.hpp"
#include "GameMath.hpp"
#include <cmath>
namespace th09 {
namespace {
void move_fraction(Bullet& b,float fraction){b.position.x=fraction*b.velocity.x+b.position.x;b.position.y=fraction*b.velocity.y+b.position.y;b.position.z=fraction*b.velocity.z+b.position.z;}
}
Bullet* BulletManager::create(const BulletEmission& e,i32 index,i32 layer,float aim,bool second_pool,const FrameTiming& timing,Rng& random,const Vec3& player,BulletFrameActions& actions){
    const u32 begin=second_pool?second_begin:0,end=begin+(second_pool?second_capacity:first_capacity);
    u32 slot=begin;while(slot<end&&pool[slot].state!=0)++slot;
    // The original exhausted-pool scan returns its first active slot without
    // creating a bullet or consuming RNG. Both fixed pools preserve slot order.
    if(slot==end)return &pool[begin];
    if(checkpoint)checkpoint->BeforeBullet(*this,slot);
    auto& b=pool[slot];
    const auto pattern=bullet_pattern(e,index,layer,aim,random);
    b.state=1;b.spawn_active=1;b.near_attack=0;b.lifetime.reset();b.collision_disabled=0;b.movement_time.reset();b.homing=0;
    b.speed=pattern.speed;b.direction=float(add_angle(pattern.angle,0));b.owner_flags=u8(e.owner_flags);b.position=e.position;b.position.z=.1f;
    const float speed=timing.rate*pattern.speed;b.velocity.x=float(std::cos(double(pattern.angle))*double(speed));b.velocity.y=float(std::sin(double(pattern.angle))*double(speed));
    b.active_extras=e.flags;b.sprite=e.sprite;b.color=e.color;b.creation_reserved=0;b.cancel_pending=0;
    if(!actions.prepare(b,slot,e.sprite,e.color,e.flags))return nullptr;
    b.transform_sound=e.transform_sound;b.offscreen_grace=0;
    if(e.flags&14){b.state=e.flags&2?2:e.flags&4?3:4;b.position.x-=4.f*b.velocity.x;b.position.y-=4.f*b.velocity.y;b.position.z-=4.f*b.velocity.z;}
    std::memcpy(b.extras,e.extras,sizeof(b.extras));b.available_extras=e.flags;b.active_extras=0;b.extra_index=e.extra_index;
    if(!BulletExtras(timing,player,actions).begin(b))return nullptr;
    if(cancel_frames&&!(b.available_extras&0x1000))b.state=5;return &b;
}
bool BulletManager::emit(const BulletEmission& e,bool second_pool,const FrameTiming& timing,Rng& random,const Vec3& player,BulletFrameActions& actions){
    const float dx=player.x-e.position.x,dy=player.y-e.position.y;
    const float aim=(dx==0&&dy==0)?1.5707963705062866f:float(std::atan2(double(dy),double(dx)));
    for(i32 layer=0;layer<e.layers;++layer)for(i32 index=0;index<e.count;++index)if(!create(e,index,layer,aim,second_pool,timing,random,player,actions))return false;
    if(e.flags&0x200)actions.play_positioned_sound(e.sound,e.position.x);return true;
}
void BulletManager::reset_pool(){
    // Round reset writes dormant slots too. Active runs may already own their
    // bytes, so the observer deduplicates by slot before recording first writes.
    for(u32 i=0;i<pool.size();++i){if(checkpoint)checkpoint->BeforeBullet(*this,i);pool[i]=Bullet{};}
    pool[first_capacity].state=pool[update_count].state=6;
}
void BulletManager::add_draw(Bullet& b,u32 index) noexcept {
    if(b.draw_group>=draw_heads.size())return;b.draw_next=draw_heads[b.draw_group];draw_heads[b.draw_group]=i32(index);
}
bool BulletManager::update_bullets(const FrameTiming& timing,const Vec3& player,u32 field_flags,u32 game_flags,BulletFrameActions& actions){
    if(game_flags&0x1800)return true;
    total=first_count=second_count=0;draw_heads.fill(-1);BulletExtras extras(timing,player,actions);
    for(u32 index=0;index<update_count;++index){
        auto& b=pool[index];if(b.state==0||b.state==6)continue;
        if(field_flags&1){add_draw(b,index);continue;}
        ++total;if(index<first_capacity)++first_count;else ++second_count;
        bool normal=b.state==1;
        if(b.state>=2&&b.state<=4){
            b.movement_time.decrement(1,timing);
            const auto animation=BulletAnimation(u32(b.state)-1);const float divisor=b.state==2?2.f:b.state==3?2.5f:3.f;
            move_fraction(b,1.f/divisor);
            if(actions.advance_animation(b,animation)){b.state=1;b.lifetime.reset();normal=true;}
        }else if(b.state==5){
            move_fraction(b,.5f);if(actions.advance_animation(b,BulletAnimation::cancel)){b.remove();continue;}
        }
        if(normal){
            if(!extras.begin(b))return false;extras.update(b);
            if(b.offscreen_grace)b.offscreen_grace=wrapping_sub(b.offscreen_grace,1);
            b.position.x=b.velocity.x+b.position.x;b.position.y=b.velocity.y+b.position.y;b.position.z=b.velocity.z+b.position.z;
            if(!b.offscreen_grace){
                if(b.intersects_field())b.outside_frames=0;
                else if(b.active_extras&0xdc0){b.outside_frames=i16(u16(b.outside_frames)+1);if(u16(b.outside_frames)>=128){b.remove();continue;}}
                else if(b.outside_frames)b.outside_frames=i16(u16(b.outside_frames)-1);
                else {b.remove();continue;}
            }
            if(!b.collision_disabled){
                bool collision=b.near_attack!=0;
                if(!collision){const i32 result=actions.probe_attacks(b);if(result==1){b.near_attack=1;collision=true;}else if(result==2&&!(b.available_extras&0x1000))b.state=5;}
                if(collision){const i32 result=actions.collide_attacks(b);if(result==0||(result==2&&(b.available_extras&0x1000)))actions.collide_player(b);else b.state=5;}
            }
            if(b.has_body_script)actions.advance_animation(b,BulletAnimation::body);
        }
        b.lifetime.tick(timing);b.movement_time.tick(timing);add_draw(b,index);
    }
    return true;
}
}
