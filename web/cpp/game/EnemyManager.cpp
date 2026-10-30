#include "EnemyManager.hpp"
#include <algorithm>
namespace th17 {
bool EnemyManager::spawn_at_rate(const EnemySpawnRequest& request,float initial_rate){
    auto enemy=std::make_unique<EnemyRuntime>(program,world,random,visual_random,host);
    const u32 identifier=next_identifier;world.total=identifier;next_identifier++;if(!next_identifier)next_identifier=1;
    enemy->commands.attributes.host=&host;enemy->commands.spawning.spawning=this;enemy->commands.scene.host=&host;enemy->commands.scene.spawning=this;
    if(!enemy->initialize(request,identifier)){error=enemy->error;return false;}
    if(enemy->step(initial_rate,background_delta)==-2){error=enemy->error;return false;}
    enemy->state.finish_spawn();world.enemies.push_back(&enemy->state);world.enemy_count=wrapping_add(world.enemy_count,1);active.push_back(std::move(enemy));world.refresh_boss();return true;
}
bool EnemyManager::boss_exists(i32 slot)const noexcept{return world.boss_at(slot)!=nullptr;}
bool EnemyManager::switch_enemy_routine(u32 identifier,const std::string& name){const auto* candidate=world.lookup(identifier);if(!candidate)return true;auto* runtime=find(candidate->id);if(!runtime){error="Enemy routine target outside manager";return false;}if(!runtime->switch_routine(name)){error=runtime->error;return false;}return true;}
bool EnemyManager::clear_field(bool suppress_death_script){
    auto next=active.begin();while(next!=active.end()){
        auto current=next++;auto& runtime=**current;auto& enemy=runtime.state;
        if((enemy.flags&0xc004a0)&&!(enemy.flags&0x100))continue;
        enemy.item_drops={};enemy.drop_radius={32,32};enemy.primary_drop=0;
        enemy.last_hit_position.x=0;enemy.last_hit_position.y=192;enemy.chapter_contribution=0;
        if(suppress_death_script)enemy.death_script.clear();
        if(!host.scene_death(runtime,world.current_rate(rate))){error=runtime.error.empty()?"Enemy scene death unavailable":runtime.error;return false;}
        auto* animations=host.animations();
        for(auto& handle:enemy.animation_handles){if(!animations||!animations->retire(handle)){error="Enemy field animation cleanup unavailable";return false;}}
        enemy.flags|=0x2000000;
    }
    const float current=world.current_rate(rate);timer.tick(&current);return true;
}
EnemyRuntime* EnemyManager::find(u32 identifier)noexcept{for(auto& enemy:active)if(enemy->state.id==identifier)return enemy.get();return nullptr;}
EnemyRuntime* EnemyManager::at(u32 index)noexcept{for(auto& enemy:active)if(index--==0)return enemy.get();return nullptr;}
bool EnemyManager::erase(std::list<std::unique_ptr<EnemyRuntime>>::iterator entry){auto* enemy=entry->get();if(!host.destroy(enemy->state)){error="Enemy cleanup failed";return false;}auto position=std::find(world.enemies.begin(),world.enemies.end(),&enemy->state);if(position!=world.enemies.end())world.enemies.erase(position);world.refresh_boss();if(!(enemy->state.lifecycle_flags&4))world.enemy_count=wrapping_add(world.enemy_count,-1);active.erase(entry);return true;}
bool EnemyManager::update(){
    if(((world.game_frame_flags>>2|world.game_frame_flags)&1)||(world.game_frame_flags&0x402))return true;
    world.shot_damage=world.other_damage=0;
    auto next=active.begin();while(next!=active.end()){auto current=next++;auto& enemy=**current;const int result=enemy.state.flags&0x2000000?-1:enemy.step(rate,background_delta);if(result==-2){error=enemy.error;return false;}if(result){if(!erase(current))return false;}else enemy.clear_update_guard();}
    if(world.player_damage_scale>1.01f)world.player_behavior_flags|=32;else world.player_behavior_flags&=~32u;world.player_damage_scale=1;
    const float current=world.current_rate(rate);timer.tick(&current);return true;
}
bool EnemyManager::clear(){while(!active.empty())if(!erase(active.begin()))return false;error.clear();return true;}
}
