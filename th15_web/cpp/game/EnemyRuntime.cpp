#include "EnemyRuntime.hpp"
#include <cmath>
namespace th15 {
EnemyRuntime::EnemyRuntime(EclProgram& program,EnemyWorldState& world,Rng& random,Rng& visual_random,EnemyRuntimeHost& host):world(world),host(host),variables(state,world,random),commands(state,world),scripts(program,&variables,&visual_random){scripts.main.commands=&commands;commands.visuals.host=host.animations();commands.lasers.scene=host.lasers();}
bool EnemyRuntime::initialize(const EnemySpawnRequest& request,u32 identifier){error.clear();state.initialize(request,identifier,world.current_chapter);if(!scripts.restart(request.routine.c_str(),u8(1u<<(u32(world.difficulty)&31)))){error=scripts.error;return false;}return true;}
bool EnemyRuntime::switch_routine(const std::string& name){if(!scripts.restart(name.c_str(),scripts.main.difficulty)){error=scripts.error;return false;}return true;}
int EnemyRuntime::step(float rate,const Vec3& background_delta){
    rate=world.current_rate(rate);scripts.main.interpolation_clock=world.frame_rate;
    auto* animations=host.animations();
    if(state.slowdown>0||state.lifecycle_flags&1){
        if(!animations){error="Enemy slowdown animation host unavailable";return -2;}
        for(auto& handle:state.animation_handles){auto* vm=animations->find(handle);if(vm)vm->slowdown=state.slowdown>0?state.slowdown:0;else handle=0;}
    }
    if(!(state.slowdown>0))return update(rate,background_delta);
    float adjusted=float(rate-float(state.slowdown*rate));if(adjusted>1)adjusted=1;else if(adjusted<0)adjusted=0;
    if(world.frame_rate)*world.frame_rate=adjusted;const int result=update(adjusted,background_delta);if(world.frame_rate)*world.frame_rate=rate;state.lifecycle_flags|=1;return result;
}
int EnemyRuntime::update(float rate,const Vec3& background_delta){
    if(state.flags&0x40000)return 0;state.flags|=0x40000;
    const auto movement=state.update_movement(rate,background_delta,host.animations());
    if(movement==EnemyMovementResult::outside)return -1;
    if(movement==EnemyMovementResult::animation_unavailable){error="Enemy movement animation unavailable";return -2;}
    if(const int result=scripts.step(rate,rate)){if(result==-2){error=scripts.error;return -2;}return -1;}
    if(const int result=host.after_script(*this,world.current_rate(rate))){if(result==-2&&error.empty())error="Enemy callback unavailable";return result==-2?-2:-1;}
    if(const int result=host.collide_and_damage(*this,world.current_rate(rate))){if(result==-2&&error.empty())error="Enemy damage integration failed";return result==-2?-2:-1;}
    if(!host.update_distortion(state,world.current_rate(rate))){error="Enemy distortion integration failed";return -2;}
    auto* animations=host.animations();
    for(u32 index=0;index<14;index++){
        auto& handle=state.animation_handles[index];auto* vm=animations?animations->find(handle):nullptr;
        if(!vm){if(!(state.flags&0x4000000))handle=0;else if(handle&&!animations){error="Persistent enemy animation host unavailable";return -2;}continue;}
        if(state.flags&0x4000000){vm->presentation_motion=true;vm->visual.translation=state.motion.position;continue;}
        const auto& offset=state.animation_offsets[index];Vec3 position{float(offset.x+state.motion.position.x),float(offset.y+state.motion.position.y),float(state.motion.position.z+offset.z)};
        const i32 parent=state.animation_parents[index];if(parent>=0){if(parent>=16){error="Enemy animation parent outside range";return -2;}auto& parent_handle=state.animation_handles[parent];auto* parent_vm=animations->find(parent_handle);if(parent_vm){const auto& anchor=parent_vm->variables.position;position={float(anchor.x+position.x),float(anchor.y+position.y),float(anchor.z+position.z)};}else parent_handle=0;}
        vm->presentation_motion=true;vm->visual.translation=position;
        if(vm->visual.render_flags&128){vm->variables.rotation.z=float(std::atan2(double(state.motion.velocity.y),double(state.motion.velocity.x)));vm->visual.flags|=4;state.rotation_angle=vm->variables.rotation.z;}
    }
    rate=world.current_rate(rate);if(state.collision_timer.current>0)state.collision_timer.decrement(&rate);
    if(state.invulnerability_timer.current>0)state.invulnerability_timer.decrement(&rate);
    state.lifetime_timer.tick(&rate);state.age_timer.tick(&rate);return 0;
}
}
