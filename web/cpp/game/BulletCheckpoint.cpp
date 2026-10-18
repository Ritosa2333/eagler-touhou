#include "BulletCheckpoint.hpp"
namespace th15 {
bool BulletCheckpoint::capture(){
    error.clear();available=false;reward_count=scene.manager.cancellation_rewards.count;reflection_bounds=scene.manager.context.reflection_bounds;
    for(u32 i=0;i<slots.size();i++){const auto* visual=scene.visual(i);auto& out=slots[i];out.state=*scene.manager.state(i);out.body=visual->body;out.overlay=visual->overlay;effects[i]=scene.cancellation_animations[i]?pool.capture(scene.cancellation_animations[i],animations.registry):0;if(!pool.error.empty()){error=pool.error;return false;}}
    available=true;return true;
}
bool BulletCheckpoint::restore(){
    error.clear();if(!available){error="No saved bullet chapter";return false;}animations.retire_resource(bank);
    auto& m=scene.manager;m.free_head=m.active_head=BulletManager::none;m.cancellation_rewards.count=reward_count;m.context.reflection_bounds=reflection_bounds;
    // The original rebuilds both intrusive lists in ascending storage order,
    // prepending each node. It does not retain the previous emission order.
    for(u32 i=0;i<slots.size();i++){const auto& in=slots[i];auto& out=m.slots[i];out.state=in.state;out.free_next=out.active_next=out.active_previous=BulletManager::none;out.active=(out.state.flags&1)!=0;
        if(out.active){out.active_next=m.active_head;if(m.active_head!=BulletManager::none)m.slots[m.active_head].active_previous=i;m.active_head=i;}
        else{out.free_next=m.free_head;m.free_head=i;}
        auto* visual=scene.visual(i);visual->body=in.body;visual->overlay=in.overlay;visual->restore_sprite_source();
    }
    for(u32 i=0;i<effects.size();i++){scene.cancellation_animations[i]=effects[i]?pool.restore(effects[i],animations.registry):0;if(!pool.error.empty()){error=pool.error;return false;}}
    return true;
}
}
