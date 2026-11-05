#include "EnemyTrail.hpp"
#include "EclVm.hpp"
namespace th08 {
bool configure_enemy_trail(EclVm& enemy,const EclInstruction& instruction){
    if(instruction.size<28){enemy.invalid=true;return false;}
    auto& trail=enemy.trail;trail.flags=u8(enemy.variable_int(instruction,0));
    trail.length=i16(enemy.operand_int(instruction,1));trail.collision_length=i16(enemy.operand_int(instruction,2));trail.step=i16(enemy.operand_int(instruction,3));
    if(trail.flags&8){
        if(!trail.step){enemy.invalid=true;return false;}
        const i32 count=trail.length/trail.step*2;if(count>194){enemy.invalid=true;return false;}
        if(count>=3&&!enemy.animation[0].loadedSprite){enemy.invalid=true;enemy.failure=EclVm::Failure::MissingAnimation;return false;}
        AnmRenderer::texture_strip(enemy.animation[0],trail.vertices,count,false);
    }
    return true;
}
bool update_enemy_trail(EclVm& enemy)noexcept{
    auto& trail=enemy.trail;if(!trail.flags)return true;
    if(trail.length>96){enemy.invalid=true;return false;}
    for(i32 i=trail.length-1;i>0;--i)trail.points[i]=trail.points[i-1];
    trail.points[0]={enemy.resolved_position,enemy.velocity,enemy.direction.z};return true;
}
}
