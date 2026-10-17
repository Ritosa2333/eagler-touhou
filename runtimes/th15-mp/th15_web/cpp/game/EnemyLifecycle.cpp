#include "EnemyState.hpp"
namespace th15 {
void EnemyState::initialize(const EnemySpawnRequest& request,u32 identifier,i32 current_chapter){
    *this=EnemyState{};hitbox=hurtbox={24,24};drop_radius={32,32};damage_multiplier=1;script_control={60,1};distortion.radius=0;
    age_timer.set(0);lifetime_timer.set(0);collision_timer.set(2);invulnerability_timer.set(0);phase_timer.set(0);
    animation_parents.fill(-1);absolute.position=request.position;
    score=request.score;primary_drop=request.item;life=initial_life=request.life;
    integer_registers=request.integers;float_registers=request.floats;temporary_registers=request.temporary;
    id=identifier;parent_id=request.parent;chapter=current_chapter;
    if(request.mirrored)flags|=0x80000;if(request.persistent)flags|=0x4000000;if(request.life>=1000)flags|=0x40000000;
}
void EnemyState::finish_spawn()noexcept{
    damage_limit=1000;death_sound=i32(id&1)+3;
    if(death_animation)return;death_animation=44;
    if(animation_resource==2)switch(animation_script){
        case 5:case 25:case 53:case 79:death_animation=40;break;
        case 15:case 91:death_animation=52;break;
        case 10:case 56:case 83:death_animation=48;break;
        case 30:death_animation=58;break;case 35:death_animation=57;break;case 40:death_animation=56;break;
    }death_animation_resource=1;
}
}
