#include "EnemyManager.hpp"
#include <algorithm>
namespace th15 {
bool EnemyManager::spawn_at_rate(const EnemySpawnRequest& request,float initial_rate){
    auto enemy=std::make_unique<EnemyRuntime>(program,world,random,visual_random,host);
    const u32 identifier=next_identifier;world.total=identifier;next_identifier++;if(!next_identifier)next_identifier=1;
    enemy->commands.host=&host;enemy->commands.spawning=this;enemy->commands.bullets.host=host.bullets();
    if(!enemy->initialize(request,identifier)){error=enemy->error;return false;}
    if(enemy->step(initial_rate,background_delta)==-2){error=enemy->error;return false;}
    enemy->state.finish_spawn();world.enemies.push_back(&enemy->state);world.enemy_count=wrapping_add(world.enemy_count,1);active.push_back(std::move(enemy));world.refresh_boss();return true;
}
bool EnemyManager::boss_exists(i32 slot)const noexcept{return world.boss_at(slot)!=nullptr;}
bool EnemyManager::switch_enemy_routine(u32 identifier,const std::string& name){const auto* candidate=world.lookup(identifier);if(!candidate)return true;auto* runtime=find(candidate->id);if(!runtime){error="Enemy routine target outside manager";return false;}if(!runtime->switch_routine(name)){error=runtime->error;return false;}return true;}
bool EnemyManager::clear_field(bool suppress_death_script){
    // Cache the next node before callbacks, preserving traversal when death
    // scripts append new enemies to the live list.
    auto next=active.begin();while(next!=active.end()){auto current=next++;auto& runtime=**current;auto& e=runtime.state;
        if((e.flags&0xc004a0u)&&!(e.flags&0x100u))continue;
        e.primary_drop=0;e.item_drops={};e.drop_radius={32,32};e.last_hit_position.x=0;e.last_hit_position.y=192;e.chapter_contribution=0;
        if(suppress_death_script)e.death_script.clear();
        if(host.scene_death(runtime,rate)==-2){error=runtime.error.empty()?"Scene enemy death service unavailable":runtime.error;return false;}
        e.flags|=0x2000000u;
    }const float current=world.current_rate(rate);timer.tick(&current);return true;
}
EnemyRuntime* EnemyManager::find(u32 identifier)noexcept{for(auto& enemy:active)if(enemy->state.id==identifier)return enemy.get();return nullptr;}
EnemyRuntime* EnemyManager::at(u32 index)noexcept{for(auto& enemy:active)if(index--==0)return enemy.get();return nullptr;}
bool EnemyManager::erase(std::list<std::unique_ptr<EnemyRuntime>>::iterator entry){auto* enemy=entry->get();if(!host.destroy(enemy->state)){error="Enemy cleanup failed";return false;}auto position=std::find(world.enemies.begin(),world.enemies.end(),&enemy->state);if(position!=world.enemies.end())world.enemies.erase(position);world.refresh_boss();if(!(enemy->state.lifecycle_flags&4))world.enemy_count=wrapping_add(world.enemy_count,-1);active.erase(entry);return true;}
bool EnemyManager::update(){
    world.shot_damage=world.other_damage=0;
    auto next=active.begin();while(next!=active.end()){auto current=next++;auto& enemy=**current;const int result=enemy.state.flags&0x2000000?-1:enemy.step(rate,background_delta);if(result==-2){error=enemy.error;return false;}if(result){if(!erase(current))return false;}else enemy.clear_update_guard();}
    const float current=world.current_rate(rate);timer.tick(&current);return true;
}
bool EnemyManager::clear(){while(!active.empty())if(!erase(active.begin()))return false;error.clear();return true;}
}
