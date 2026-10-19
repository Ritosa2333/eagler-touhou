#include "GameRuntime.hpp"

namespace th08 {
void GameRuntime::reset() noexcept {
    frame=0;enemy_running=false;timeline_running=false;stage_loaded=false;ecl_loaded=false;combat.reset();player.reset();enemies.reset();requests.clear();globals=EclGlobals{};values=GameGlobals{};globals.combat=&combat;globals.values=&values;globals.timeline_actions=this;stage.reset();ecl.release();enemy_vm=EclVm{};timeline_vm=EclTimelineVm{};random=Rng{};timing=FrameTiming{};
    enemy_animations.release(7);enemy_animations.release(8);
    player_shots.reset();
    emitted_bullets.clear();globals.bullet_actions=this;globals.phase_actions=this;globals.laser_actions=this;globals.enemy_actions=this;enemy_effects=nullptr;
    projectiles->reset();enemy_animations.release(6);projectiles_loaded=false;projectile_failure=false;projectile_replay_flags=0;projectile_requests.clear();
}
bool GameRuntime::load_enemy_animation(i32 slot,const u8* bytes,u32 size){
    if(slot<0||slot>1||enemy_running||enemies.active_count())return false;
    globals.enemy_animation_files[slot]=enemy_animations.load(slot+7,bytes,size);
    return globals.enemy_animation_files[slot]!=nullptr;
}
bool GameRuntime::load_projectile_animation(const u8* bytes,u32 size){
    if(projectiles_loaded||enemy_running||enemies.active_count())return false;
    auto* file=enemy_animations.load(6,bytes,size);if(!file)return false;
    projectiles->animation=file;projectiles_loaded=projectiles->templates.load(*file,random,timing);return projectiles_loaded;
}
void GameRuntime::emit(BulletEmission& parameters){
    if(projectiles_loaded){bullet_creation.timing=timing;bullet_creation.emit(parameters,bullet_aim(parameters.position,globals.player),&projectile_replay_flags);projectile_failure|=bullet_creation.failure!=BulletCreation::Failure::None;}
    emitted_bullets.push_back(parameters);
}
LaserState* GameRuntime::laser(BulletEmission& parameters){
    if(!projectiles_loaded)return nullptr;laser_runtime.timing=timing;laser_runtime.player=globals.player;
    auto* result=laser_runtime.create(parameters);projectile_failure|=laser_runtime.invalid;return result;
}
void GameRuntime::clear(i32 mode){
    requests.push_back({3,mode});if(projectiles_loaded)projectile_failure|=!cancel_projectiles(*projectiles,mode,-1,this);
}
bool GameRuntime::load_ecl(const u8* bytes,u32 size){enemies.reset();ecl_loaded=ecl.load(bytes,size);enemy_running=false;timeline_running=false;return ecl_loaded;}
bool GameRuntime::load_stage(const u8* bytes,u32 size,i32 index,bool practice){const bool result=stage.load(bytes,size,index,practice);stage_loaded=stage.ready();return result;}
bool GameRuntime::start_timeline(i32 index,u32 difficulty){if(!ecl_loaded)return false;globals.difficulty=difficulty;globals.difficulty_mask=1u<<(difficulty<8?difficulty:7);timeline_running=timeline_executor.start(timeline_vm,ecl,index,difficulty);return timeline_running;}
bool GameRuntime::start_enemy_subroutine(i32 index){if(!ecl_loaded)return false;enemy_running=executor.start(enemy_vm,ecl,index);return enemy_running;}
bool GameRuntime::update(u16 buttons){
    if(!stage_loaded&&!ecl_loaded)return false;
    ++frame;projectile_failure=false;player.step(buttons,timing.rate);globals.player=player.status().position;globals.youkai=player.status().focused;
    if(stage_loaded){if(globals.stage_interrupt){stage.interrupt(globals.stage_interrupt);globals.stage_interrupt=0;}
        if(!stage.update(timing,globals.paused,player.status().focused)){stage_loaded=false;return false;}}
    if(timeline_running&&!globals.paused){timeline_executor.step(timeline_vm);timeline_running=!timeline_vm.finished&&!timeline_vm.invalid;}
    if(!globals.paused)enemies.update_scripts(timing);
    if(enemy_running){executor.step(enemy_vm);enemy_running=!enemy_vm.finished&&!enemy_vm.invalid;if(enemy_running&&!globals.paused)enemy_vm.integrate_position(timing);}
    globals.player=player.status().position;combat.player_state().position=globals.player;combat.update(timing.rate);
    if(projectiles_loaded&&!(globals.game_flags&0x400)){
        BulletUpdate update(*projectiles,bullet_creation,random);update.actions=this;update.timing=timing;update.player=globals.player;update.paused=globals.paused;
        if(!update.update_bullets())return false;laser_runtime.timing=timing;laser_runtime.actions=laser_collision_actions;if(!laser_runtime.update())return false;
        if(projectiles->cancel_frames)projectiles->cancel_frames=wrapping_sub(projectiles->cancel_frames,1);projectiles->timer.tick(timing);projectiles->unknown_counter=wrapping_add(projectiles->unknown_counter,1);
    }
    return !projectile_failure;
}
}
