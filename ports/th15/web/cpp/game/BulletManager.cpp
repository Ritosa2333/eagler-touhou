#include "BulletManager.hpp"
namespace th15 {
BulletManager::BulletManager(BulletManagerHost& h):slots(capacity),host(h){reset();}
void BulletManager::reset(){
    free_head=active_head=none;error.clear();visible_count=0;minimum_distance_squared=0;cancellation_rewards.count=0;
    for(auto& group:draw_groups)group.clear();
    for(u32 i=0;i<capacity;i++){
        auto& slot=slots[i];slot.state=BulletState{};auto& s=slot.state;const Timer zero{0,0,0,0,0};
        s.boost=s.acceleration.timer=s.approach.timer=s.angular.timer=s.drift.timer=s.position_curve.timer=s.turn.timer=s.wait=s.invulnerability=s.freeze=s.lifetime=s.spawn=s.graze_flash=s.graze_duration=zero;
        s.reflection.bounds={};s.transform_sound=0;s.phase=0;s.scale=0;s.cancellation_script=0;s.graze_interval=0;
        slot.free_next=free_head;slot.active_next=slot.active_previous=none;slot.active=false;free_head=i;
    }
}
u32 BulletManager::acquire()noexcept{
    if(free_head==none)return none;const u32 index=free_head;auto& slot=slots[index];free_head=slot.free_next;slot.free_next=none;slot.active=true;slot.active_previous=none;slot.active_next=active_head;if(active_head!=none)slots[active_head].active_previous=index;active_head=index;return index;
}
void BulletManager::recycle(u32 index)noexcept{
    if(index>=capacity)return;auto& slot=slots[index];auto& state=slot.state;if(!state.phase)return;
    state.phase=0;state.spawn.set(0);state.lifetime.set(0);state.graze_flash.set(0);state.graze_duration.set(0);state.flags&=0xfffffcbeu;state.step_limit=0;
    if(slot.active_previous==none)active_head=slot.active_next;else slots[slot.active_previous].active_next=slot.active_next;
    if(slot.active_next!=none)slots[slot.active_next].active_previous=slot.active_previous;
    slot.active_next=slot.active_previous=none;slot.active=false;slot.free_next=free_head;free_head=index;
}
bool BulletManager::emit(const BulletShooter& shooter,float minimum){
    if(!random){error="Bullet manager random state unavailable";return false;}
    struct DistanceScope {float& value;float previous;~DistanceScope(){value=previous;}} distance_scope{minimum_distance_squared,minimum_distance_squared};minimum_distance_squared=minimum;
    const float aim=bullet_aim(shooter.position,context.player);
    for(i32 row=0;row<shooter.rows;row++){
        for(i32 column=0;column<shooter.count;column++){
            const u32 index=acquire();if(index==none)goto finished;
            if(!slots[index].state.initialize(shooter,column,row,aim,context.player,minimum,*random,host.bullet(index),error)&&!error.empty()){recycle(index);return false;}
            if(const auto* failure=host.failure();failure&&!failure->empty()){error=*failure;return false;}
        }
    }
finished:
    if(shooter.flags&32)host.play(shooter.shoot_sound);return true;
}
bool BulletManager::update(){
    if(!random){error="Bullet manager random state unavailable";return false;}
    error.clear();visible_count=0;for(auto& group:draw_groups)group.clear();
    for(u32 index=active_head;index!=none;){
        auto& slot=slots[index];const u32 next=slot.active_next;auto& state=slot.state;auto& frame_host=host.bullet(index);
        if(!world_paused){
            if((state.flags&256)&&(state.phase==1||(state.phase==2&&state.spawn.current>=8)))frame_host.contact(state,true);
            else{auto frame_context=context;host.prepare(index,frame_context);const auto result=state.update(frame_context,*random,frame_host,error);if(result==BulletFrameResult::error)return false;if(result==BulletFrameResult::recycled){index=next;continue;}}
        }
        if(const auto* failure=host.failure();failure&&!failure->empty()){error=*failure;return false;}
        if(!(state.flags&512)){if(state.cancel_item<0||state.cancel_item>=6){error="Invalid bullet draw group";return false;}draw_groups[state.cancel_item].push_back(index);}
        visible_count++;state.spawn.tick(&context.rate);index=next;
    }
    return true;
}
}
