#include "EnemyCheckpoint.hpp"
namespace th15 {
bool EnemyCheckpoint::capture(){
    error.clear();available=false;entries.clear();auto& world=manager.world;
    integer_registers=world.integer_registers;float_registers=world.float_registers;counters=world.counters;boss_ids=world.boss_ids;next_id=manager.next_identifier;total=world.total;control=world.enemy_control;manager_flags=world.manager_flags;shot_damage=world.shot_damage;other_damage=world.other_damage;timer=manager.timer;
    entries.reserve(manager.active.size());
    for(auto& enemy:manager.active){entries.push_back({enemy->state,enemy->scripts.snapshot()});auto& saved=entries.back();
        for(u32 i=0;i<saved.state.animation_handles.size();i++){auto& handle=saved.state.animation_handles[i];if(!animations.registry.find(handle)){enemy->state.animation_handles[i]=0;handle=0;}else handle=pool.capture(handle,animations.registry);if(!pool.error.empty()){error=pool.error;return false;}}
        auto save_grid_handle=[&](u32& handle)->bool{if(!animations.registry.find(handle))handle=0;else handle=pool.capture(handle,animations.registry);if(!pool.error.empty()){error=pool.error;return false;}return true;};
        if(!save_grid_handle(saved.state.distortion_mesh.capture_handle))return false;for(auto& handle:saved.state.distortion_mesh.strip_handles)if(!save_grid_handle(handle))return false;
        saved.state.lifecycle_flags|=4;
    }available=true;return true;
}
bool EnemyCheckpoint::restore(){
    error.clear();if(!available){error="No saved enemy chapter";return false;}
    if(!manager.clear()){error=manager.error;return false;}for(auto id:banks)if(id>=0)animations.retire_resource(id);
    auto& world=manager.world;world.integer_registers=integer_registers;world.float_registers=float_registers;world.counters=counters;world.boss_ids=boss_ids;world.total=total;world.enemy_control=control;world.manager_flags=manager_flags;world.shot_damage=shot_damage;world.other_damage=other_damage;world.enemy_count=0;manager.next_identifier=next_id;manager.timer=timer;
    for(const auto& saved:entries){auto enemy=std::make_unique<EnemyRuntime>(manager.program,world,manager.random,manager.visual_random,manager.host);enemy->commands.host=&manager.host;enemy->commands.spawning=&manager;enemy->commands.bullets.host=manager.host.bullets();enemy->state=saved.state;enemy->state.lifecycle_flags&=~4u;
        for(auto& handle:enemy->state.animation_handles){handle=handle?pool.restore(handle,animations.registry):0;if(!pool.error.empty()){error=pool.error;return false;}}
        auto restore_grid_handle=[&](u32& handle)->bool{handle=handle?pool.restore(handle,animations.registry):0;if(!pool.error.empty()){error=pool.error;return false;}return true;};
        if(!restore_grid_handle(enemy->state.distortion_mesh.capture_handle))return false;for(auto& handle:enemy->state.distortion_mesh.strip_handles)if(!restore_grid_handle(handle))return false;
        enemy->scripts.main.interpolation_clock=world.frame_rate;if(!enemy->scripts.restore(saved.scripts)){error=enemy->scripts.error;return false;}
        world.enemies.push_back(&enemy->state);manager.active.push_back(std::move(enemy));world.enemy_count=wrapping_add(world.enemy_count,1);
    }world.refresh_boss();return true;
}
}
