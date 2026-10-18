#include "EnemyManager.hpp"
#include "Player.hpp"
namespace th10 {
// 0x40cc70. Construction leaves the surrounding storage untouched.
void EnemyState::construct() noexcept {
    lifetime_flags&=~1u;absolute_position.flags&=~1u;relative_position.flags&=~1u;
    absolute_angle.flags&=~1u;relative_angle.flags&=~1u;absolute_radius.flags&=~1u;relative_radius.flags&=~1u;
    for(auto& emitter:emitters)emitter.initialize();damage_immunity_flags&=~1u;collision_immunity_flags&=~1u;
}
// 0x40d830. The original clears the enemy state after construction, including
// the emitter defaults. The script stack bytes and difficulty upper bytes stay.
void Enemy::initialize(EclProgram& program,const char* subroutine,const float* rate,void* type_table) noexcept {
    script.construct(type_table);
    __builtin_memset(&state,0,sizeof(state));script.initialize_context();state.script_owner=&script;
    state.hitbox=state.collision_box={24,24};state.boss_slot=-1;state.manager_node.initialize(this);state.drops.spread={32,32};
    state.lifetime={-1,0,0,rate};state.lifetime_flags=1;
    state.damage_immunity={-1,0,0,rate};state.damage_immunity_flags=1;
    state.collision_immunity={-1,0,0,rate};state.collision_immunity_flags=1;
    script.program=&program;script.select_subroutine(subroutine);
    for(auto& interrupt:state.interrupts){interrupt.health=interrupt.time=-1;}
}
// 0x40dae0. Teardown is distinct from the gameplay death/drop transition.
void Enemy::shutdown(EnemyManager& manager,EnemyManagerEnvironment& env){
    script.original_virtual_table=env.enemy_type_table;auto& node=state.manager_node;
    if(manager.head==&node)manager.head=node.next;if(manager.tail==&node)manager.tail=node.previous;node.unlink();manager.count=wrapping_add(manager.count,-1);
    if(state.flags&0x8000)manager.bosses[state.boss_slot]=nullptr;
    for(auto& id:state.animations)env.registry->delete_and_clear(id);
    if(env.player)env.player->forget_enemy(this);
    script.original_virtual_table=env.script_type_table;script.release_threads(*env.scripts);
}
}
