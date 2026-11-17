#pragma once
#include "../../cpp/game/EnemyDeath.hpp"
struct EnemyDeathFixture:EnemyRuntimeFixture,EnemyDeathHost {
    std::vector<ItemSpawnRequest> items;std::vector<EnemyDeathEffect> effects;
    explicit EnemyDeathFixture(EclProgram& program):EnemyRuntimeFixture(program){}
    bool spawn_item(const ItemSpawnRequest& request)override{events.insert(events.end(),{10,u32(request.type)});items.push_back(request);return true;}
    bool effect(const EnemyDeathEffect& request)override{events.insert(events.end(),{11,u32(request.script)});effects.push_back(request);return true;}
    bool sound(i32 id,const Vec3& position)override{return EnemyRuntimeFixture::sound(id,position);}
    bool callback(EnemyRuntime&)override{events.insert(events.end(),{12,0});return true;}
};
API(enemy_death_create) EnemyDeathFixture* enemy_death_create(EclProgram* program){return new EnemyDeathFixture(*program);}
API(enemy_death_delete) void enemy_death_delete(EnemyDeathFixture* p){delete p;}
API(enemy_death_step) i32 enemy_death_step(EnemyDeathFixture* p,float rate){p->runtime.state=p->projection.enemy;p->events.clear();p->items.clear();p->effects.clear();EnemyDeath death(p->projection.world,p->projection.random,*p);const int result=death.execute(p->runtime,rate);p->projection.enemy=p->runtime.state;return result;}
API(enemy_drop_items) u32 enemy_drop_items(EnemyDeathFixture* p){p->events.clear();p->items.clear();return drop_enemy_items(p->projection.enemy,p->projection.random,*p);}
API(enemy_drop_items_now) u32 enemy_drop_items_now(EnemyDeathFixture* p){p->events.clear();p->items.clear();return drop_enemy_items_now(p->projection.enemy,p->projection.random,*p);}
API(enemy_death_items) const ItemSpawnRequest* enemy_death_items(EnemyDeathFixture* p){return p->items.data();}
API(enemy_death_item_count) u32 enemy_death_item_count(EnemyDeathFixture* p){return p->items.size();}
API(enemy_death_effects) const EnemyDeathEffect* enemy_death_effects(EnemyDeathFixture* p){return p->effects.data();}
API(enemy_death_effect_count) u32 enemy_death_effect_count(EnemyDeathFixture* p){return p->effects.size();}
API(enemy_set_death_script) void enemy_set_death_script(EnemyVariableFixture* p,const char* name){p->enemy.death_script=name;}
API(enemy_death_angle) float* enemy_death_angle(EnemyVariableFixture* p){return &p->world.last_death_angle;}
