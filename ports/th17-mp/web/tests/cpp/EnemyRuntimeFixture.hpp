#pragma once
#include "../../cpp/game/EnemyRuntime.hpp"
#include "../../cpp/game/EnemyDamage.hpp"
struct EnemyRuntimeFixture:EnemyRuntimeHost,EnemyDamageHost {
    EnemyVariableFixture projection;Rng visual_random;EnemyRuntime runtime;std::vector<u32> events;i32 callback_result=0,damage_result=0;
    explicit EnemyRuntimeFixture(EclProgram& program):runtime(program,projection.world,projection.random,visual_random,*this){projection.visuals=std::make_unique<CaptureEnemyVisual>();runtime.commands.visuals.host=projection.visuals.get();runtime.commands.attributes.host=&projection.events;}
    EnemyVisualHost* animations()override{return projection.visuals.get();}
    void record(u32 event,float rate){u32 bits;std::memcpy(&bits,&rate,4);events.insert(events.end(),{event,bits});}
    int after_script(EnemyRuntime&,float rate)override{record(1,rate);return callback_result;}
    bool use_real_damage=false;
    int collide_and_damage(EnemyRuntime& entity,float rate)override{record(2,rate);if(use_real_damage){EnemyDamage damage(projection.world,*this,*animations());return damage.update(entity,rate);}return damage_result;}
    bool update_distortion(EnemyState&,float rate)override{record(3,rate);return true;}
    std::array<i32,5> damage_configuration{};
    bool shot_damage(EnemyState&,bool special,EnemyShotDamage& hit)override{events.insert(events.end(),{4,u32(special)});hit.amount=damage_configuration[0];hit.direct=damage_configuration[1]!=0;return true;}
    bool additional_damage(EnemyState&,i32 incoming,i32& amount)override{events.insert(events.end(),{5,u32(incoming)});amount=damage_configuration[2];return true;}
    int die(EnemyRuntime&,float)override{events.insert(events.end(),{6,0});return damage_configuration[3];}
    bool contact(EnemyState&,AnmVm*,i32& result)override{events.insert(events.end(),{7,0});result=damage_configuration[4];return true;}
    bool graze(const Vec3&)override{events.insert(events.end(),{8,0});return true;}
    bool sound(i32 value,const Vec3&)override{events.insert(events.end(),{9,u32(value)});return true;}
};
API(enemy_runtime_create) EnemyRuntimeFixture* enemy_runtime_create(EclProgram* program){return new EnemyRuntimeFixture(*program);}

API(enemy_runtime_delete) void enemy_runtime_delete(EnemyRuntimeFixture* p){delete p;}
API(enemy_runtime_projection) EnemyVariableFixture* enemy_runtime_projection(EnemyRuntimeFixture* p){return &p->projection;}
API(enemy_runtime_start) u32 enemy_runtime_start(EnemyRuntimeFixture* p,const char* name,const u8* bytes,u32 identifier){auto request=spawn_request(bytes);request.routine=name;const bool result=p->runtime.initialize(request,identifier);p->projection.enemy=p->runtime.state;return result;}
API(enemy_runtime_step) i32 enemy_runtime_step(EnemyRuntimeFixture* p,float rate,float x,float y,float z,u32 begin){p->runtime.state=p->projection.enemy;if(begin)p->runtime.clear_update_guard();p->events.clear();const i32 result=p->runtime.step(rate,{x,y,z});p->projection.enemy=p->runtime.state;return result;}
API(enemy_runtime_error) const char* enemy_runtime_error(EnemyRuntimeFixture* p){return p->runtime.error.c_str();}
API(enemy_runtime_context) EclContext* enemy_runtime_context(EnemyRuntimeFixture* p,u32 index){return p->runtime.scripts.at(index);}
API(enemy_runtime_threads) u32 enemy_runtime_threads(EnemyRuntimeFixture* p){return p->runtime.scripts.count();}
API(enemy_runtime_slowdown) float* enemy_runtime_slowdown(EnemyRuntimeFixture* p){return &p->projection.enemy.slowdown;}
API(enemy_runtime_results) void enemy_runtime_results(EnemyRuntimeFixture* p,i32 callback,i32 damage){p->callback_result=callback;p->damage_result=damage;}
API(enemy_runtime_event_count) u32 enemy_runtime_event_count(EnemyRuntimeFixture* p){return p->events.size()/2;}
API(enemy_runtime_events) const u32* enemy_runtime_events(EnemyRuntimeFixture* p){return p->events.data();}
API(enemy_damage_field) void* enemy_damage_field(EnemyVariableFixture* p,u32 kind){auto& e=p->enemy;switch(kind){case 0:return &e.pending_damage;case 1:return &e.cumulative_damage;case 2:return &e.damage_flash_frames;case 3:return &e.inactive_animation;case 4:return &e.normal_animation;case 5:return &e.damage_timer;case 6:return &e.last_hit_position;default:return &e.life_threshold;}}
API(enemy_damage_world) i32* enemy_damage_world(EnemyVariableFixture* p,u32 kind){auto& w=p->world;switch(kind){case 0:return &w.player_damage_state;case 1:return &w.damage_disabled;case 2:return &w.shot_damage;default:return &w.other_damage;}}
API(enemy_damage_scale) float* enemy_damage_scale(EnemyVariableFixture* p){return &p->world.player_damage_scale;}
API(enemy_damage_configuration) i32* enemy_damage_configuration(EnemyRuntimeFixture* p){return p->damage_configuration.data();}
API(enemy_runtime_use_damage) void enemy_runtime_use_damage(EnemyRuntimeFixture* p,u32 enabled){p->use_real_damage=enabled!=0;}
API(enemy_damage_step) i32 enemy_damage_step(EnemyRuntimeFixture* p,float rate){p->runtime.state=p->projection.enemy;p->events.clear();EnemyDamage damage(p->projection.world,*p,*p->projection.visuals);const int result=damage.update(p->runtime,rate);p->projection.enemy=p->runtime.state;return result;}
API(enemy_life_threshold) i32* enemy_life_threshold(EnemyVariableFixture* p){return &p->enemy.life_threshold;}
API(enemy_apply_damage) void enemy_apply_damage(EnemyVariableFixture* p,i32 amount){p->enemy.apply_damage(amount);}
