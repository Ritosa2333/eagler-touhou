#pragma once
#include "../../cpp/game/EnemyManager.hpp"
struct EnemyManagerFixture:EnemyManagerHost {
    EnemyWorldState world;Rng random,visual_random;CaptureEnemyVisual visuals;EnemyVariableFixture projection;std::vector<u32> destroyed,scene_deaths;EnemyManager manager;
    explicit EnemyManagerFixture(EclProgram& program):manager(program,world,random,visual_random,*this){world.random=&random;world.enemy_control=1000;}
    EnemyVisualHost* animations()override{return &visuals;}
    int after_script(EnemyRuntime&,float)override{return 0;}int collide_and_damage(EnemyRuntime&,float)override{return 0;}bool update_distortion(EnemyState&,float)override{return true;}
    bool pause_animation(u32,bool)override{return true;}bool sound(i32,const Vec3&)override{return true;}
    bool destroy(EnemyState& state)override{destroyed.push_back(state.id);return true;}
    bool scene_death(EnemyRuntime& runtime,float)override{scene_deaths.push_back(runtime.state.id);return true;}
};
API(enemy_manager_create) EnemyManagerFixture* enemy_manager_create(EclProgram* program){return new EnemyManagerFixture(*program);}
API(enemy_manager_delete) void enemy_manager_delete(EnemyManagerFixture* p){delete p;}
API(enemy_manager_spawn) u32 enemy_manager_spawn(EnemyManagerFixture* p,const char* name,const u8* bytes){auto request=spawn_request(bytes);request.routine=name;return p->manager.spawn(request);}
API(enemy_manager_step) u32 enemy_manager_step(EnemyManagerFixture* p,float rate){p->manager.rate=rate;return p->manager.update();}
API(enemy_manager_clear) u32 enemy_manager_clear(EnemyManagerFixture* p){return p->manager.clear();}
API(enemy_manager_clear_field) u32 enemy_manager_clear_field(EnemyManagerFixture* p,u32 suppress){return p->manager.clear_field(suppress!=0);}
API(enemy_manager_apply_projection) void enemy_manager_apply_projection(EnemyManagerFixture* p,u32 index){if(auto* runtime=p->manager.at(index))runtime->state=p->projection.enemy;}
API(enemy_manager_scene_deaths) const u32* enemy_manager_scene_deaths(EnemyManagerFixture* p){return p->scene_deaths.data();}
API(enemy_manager_scene_death_count) u32 enemy_manager_scene_death_count(EnemyManagerFixture* p){return p->scene_deaths.size();}
API(enemy_manager_visual_vm) AnmVm* enemy_manager_visual_vm(EnemyManagerFixture* p,u32 index){return &p->visuals.instances[index];}
API(enemy_manager_rate) float* enemy_manager_rate(EnemyManagerFixture* p){return &p->manager.rate;}
API(enemy_manager_next_id) u32* enemy_manager_next_id(EnemyManagerFixture* p){return &p->manager.next_identifier;}
API(enemy_manager_count) u32 enemy_manager_count(EnemyManagerFixture* p){return p->manager.count();}
API(enemy_manager_timer) Timer* enemy_manager_timer(EnemyManagerFixture* p){return &p->manager.timer;}
API(enemy_manager_world_counts) i32* enemy_manager_world_counts(EnemyManagerFixture* p,u32 index){return index==0?&p->world.enemy_count:reinterpret_cast<i32*>(&p->world.total);}
API(enemy_manager_context) EclContext* enemy_manager_context(EnemyManagerFixture* p,u32 index,u32 thread){auto* enemy=p->manager.at(index);return enemy?enemy->scripts.at(thread):nullptr;}
API(enemy_manager_projection) EnemyVariableFixture* enemy_manager_projection(EnemyManagerFixture* p,u32 index){auto* enemy=p->manager.at(index);if(!enemy)return nullptr;p->projection.enemy=enemy->state;p->projection.world=p->world;return &p->projection;}
API(enemy_manager_slowdown) float* enemy_manager_slowdown(EnemyManagerFixture* p,u32 index){auto* enemy=p->manager.at(index);return enemy?&enemy->state.slowdown:nullptr;}
API(enemy_manager_error) const char* enemy_manager_error(EnemyManagerFixture* p){return p->manager.error.c_str();}
API(enemy_manager_destroyed) const u32* enemy_manager_destroyed(EnemyManagerFixture* p){return p->destroyed.data();}
API(enemy_manager_destroyed_count) u32 enemy_manager_destroyed_count(EnemyManagerFixture* p){return p->destroyed.size();}
API(enemy_spawn_count) u32 enemy_spawn_count(EnemyVariableFixture* p){return p->spawning.count;}
API(enemy_spawn_boss) void enemy_spawn_boss(EnemyVariableFixture* p,u32 present){p->spawning.boss=present!=0;}
API(enemy_spawn_name) const char* enemy_spawn_name(EnemyVariableFixture* p){return p->spawning.last.routine.c_str();}
API(enemy_spawn_bytes) const u8* enemy_spawn_bytes(EnemyVariableFixture* p){static std::array<u8,84> bytes;bytes={};const auto& r=p->spawning.last;std::memcpy(bytes.data(),&r.position,12);const i32 fields[]={r.score,r.item,r.life,i32(r.mirrored),i32(r.persistent)};std::memcpy(bytes.data()+12,fields,20);std::memcpy(bytes.data()+32,r.integers.data(),16);std::memcpy(bytes.data()+48,r.floats.data(),16);std::memcpy(bytes.data()+64,r.temporary.data(),16);std::memcpy(bytes.data()+80,&r.parent,4);return bytes.data();}
