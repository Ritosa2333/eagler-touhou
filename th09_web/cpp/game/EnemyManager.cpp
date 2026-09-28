#include "EnemyManager.hpp"
#include <algorithm>
namespace th09 {
namespace {
void clone_prototype(EclVm& target,const EclVm& source){
    target=EclVm{};target.values=source.values;target.primary=source.primary;target.behavior_flags=source.behavior_flags;target.difficulty_flags=source.difficulty_flags;
    std::copy_n(source.interrupt_subroutines,32,target.interrupt_subroutines);target.pending_interrupt=source.pending_interrupt;
    target.position_offset=source.position_offset;target.velocity=source.velocity;target.movement=source.movement;
    target.emitter=source.emitter;target.lasers=source.lasers;target.animation=source.animation;target.status=source.status;target.trail=source.trail;target.bind_context();
}
}
EnemyManager::EnemyManager(EclWorldState& w,EclPlayfieldState& f,EclPlayfieldState& other):world(w),field(f),opponent(other){
    timeline.time=timeline.wait=Timer{0,0,0};
    auto& p=prototype;p.values.world=&world;p.values.field=&field;p.values.opponent=&opponent;p.bind_context();
    p.behavior_flags=0x4d;p.values.life=1;p.values.score_reward=100;p.values.lifetime.reset();p.pending_interrupt=-1;
    for(auto& a:p.animation.layers)std::memset(&a,0,sizeof(a));p.animation.layers[1].scriptIndex=p.animation.layers[2].scriptIndex=-1;
    p.animation.idle=p.animation.left=p.animation.right=-1;p.movement.hitbox={24,24,24};p.movement.player_protect_squared=1024;
    p.status.death_subroutine=-1;p.status.timeout=-1;p.status.motion_range={-.15f,.15f};
    for(auto& v:p.values.life_thresholds)v=-1;
    p.emitter.parameters=BulletEmission{};p.emitter.parameters.sound=7;p.emitter.parameters.transform_sound=25;p.emitter.time.reset();
    p.lasers.parameters=BulletEmission{};p.lasers.parameters.transform_sound=0;
    for(auto& point:p.trail.history)point.position.x=-999;
    for(auto& list:draw_lists)list.reserve(capacity);
}
bool EnemyManager::run_script(EclVm& enemy){
    if(checkpoint)checkpoint->BeforeEnemy(enemy);
    EclExecutor executor(timing,frame_step,difficulty_mask,bindings);if(!executor.step(enemy))return false;
    enemy.movement.update_velocity(enemy,timing,frame_step);
    if(player&&(enemy.values.flags&0x1c0)){
        if(!(enemy.values.flags&0x1000)&&player->focus&&CharacterCapture::contains(field.character,field.player,enemy.values.position,player->capture)){
            enemy.values.flags|=0x1000;
            if(!bindings.animations||!bindings.animations->start_animation(enemy,0,false,i32((enemy.values.flags&0x1c0)>>6)+0x16))return false;
            enemy.animation.left=-1;
            if(frame_actions)frame_actions->sound_pan(47,field.side?500:-500);
        }
        if((enemy.values.flags&0x1000)&&!CharacterCapture::update_motion(enemy,field.character,timing))return false;
    }
    if(!bindings.bullets||!enemy.emitter.update(enemy,timing,*bindings.bullets))return false;
    if(!bindings.animations||!enemy.animation.update_pose(enemy,*bindings.animations))return false;
    return true;
}
EclVm* EnemyManager::create(const EnemySpawn& request,const EclLocals* inherited){
    u32 index=0;while(index<capacity&&(enemies[index].behavior_flags&1))++index;
    auto& enemy=enemies[index];allocation_failed=index==capacity;if(allocation_failed)return &enemy;
    if(checkpoint)checkpoint->BeforeEnemy(enemy);
    clone_prototype(enemy,prototype);enemy.status.index=i32(index);
    if(!inherited)enemy.behavior_flags=(enemy.behavior_flags&~0x8000u)|((u32(request.mirrored)&1)<<15);
    if(request.life>=0)enemy.values.life=request.life;
    enemy.values.position=request.position;if(!inherited&&request.mirrored)enemy.values.position.x=-enemy.values.position.x;
    EclExecutor executor(timing,frame_step,difficulty_mask,bindings);
    auto& source=request.character_program?character_program:common_program;
    bool ok=executor.start(enemy,source,request.script);enemy.program=&common_program;
    if(inherited)enemy.primary.locals=*inherited;
    if(ok)ok=run_script(enemy);
    if(!ok){enemy.behavior_flags&=~1u;allocation_failed=true;return &enemy;}
    enemy.status.saved_color=u32(enemy.animation.layers[0].color1.d3dColor);enemy.values.item_reward=i8(request.item);
    // The child-spawn path restores the requested life after the initial script;
    // ordinary timeline spawns retain any life changes made by that script.
    if(inherited&&request.life>=0)enemy.values.life=request.life;
    if(request.score>=0)enemy.values.score_reward=request.score;
    enemy.status.initial_life=enemy.status.maximum_life=enemy.values.life;
    return &enemy;
}
}
