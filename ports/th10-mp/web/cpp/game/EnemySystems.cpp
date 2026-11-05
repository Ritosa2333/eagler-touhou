#include "EnemySystems.hpp"
namespace th10 {
// 0x40d280. ECL and ANM files are loaded before either frame callback is
// registered. The original deliberately ignores the script loader's result.
i32 EnemyManager::start(const char* script,EnemySystemsEnvironment& env){
    animation_files[0]=env.bullet_animation_file;
    program=static_cast<EclProgram*>(env.allocate(sizeof(EclProgram)));program->initialize(env.program_type_table);
    env.resources->enemy_animations=animation_files;program->load(script,*env.resources);
    update_handle=env.chain->add(env.update_callback,this,18,false,false,*env.callbacks);
    draw_handle=env.chain->add(env.draw_callback,this,20,true,false,*env.callbacks);
    if(!(lifetime_flags&1)){lifetime.rate=env.rate;lifetime_flags|=1;}
    lifetime.initialize(-1);return 0;
}
// 0x40d510 and the final part of 0x409f90.
void EnemyManager::activate(bool enabled) noexcept {
    if(update_handle){if(enabled)update_handle->flags|=2;else update_handle->flags&=~2u;}
    if(draw_handle){if(enabled)draw_handle->flags|=2;else draw_handle->flags&=~2u;}
}
// 0x409f90. Resetting a stage discards animation instances before destroying
// every enemy. This is distinct from the filtered death-effect command.
void EnemyManager::clear_all(EnemySystemsEnvironment& env){
    for(u32 slot=9;slot<13;++slot)env.discard_file_animations(env.animation_slots[slot]);
    auto* node=head;while(node){auto* next=node->next;if(node->value)env.enemies->destroy_enemy(*node->value);node=next;}
    spawn_count=0;bosses[0]=nullptr;activate(false);
}
// 0x40d530. Replay/transition flags retain the shared animation resources.
void EnemyManager::shutdown(EnemySystemsEnvironment& env){
    clear_all(env);env.chain->remove_locked(update_handle,*env.callbacks);env.chain->remove_locked(draw_handle,*env.callbacks);
    if(program){program->release_files(*env.resources);program->release_lookup(*env.resources,env.generic_program_type_table);env.release(program);}program=nullptr;
    if(!(*env.game_flags&9))for(u32 slot=9;slot<13;++slot)if(env.animation_slots[slot]){
        env.unload_animation(*env.animation_slots[slot]);env.release(env.animation_slots[slot]);env.animation_slots[slot]=nullptr;
    }
    *env.active_enemies=nullptr;
}
// 0x40d6b0.
EnemyManager* EnemyManager::create(const char* script,EnemySystemsEnvironment& env){
    auto* manager=static_cast<EnemyManager*>(env.allocate(sizeof(EnemyManager)));manager->initialize();*env.active_enemies=manager;
    if(manager->start(script,env)){manager->shutdown(env);env.release(manager);return nullptr;}return manager;
}
}
