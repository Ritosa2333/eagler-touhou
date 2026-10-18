#include "EnemyCommands.hpp"
namespace th10 {
// 0x40e760 / 0x40e770. All non-default entries of the original enemy ECL
// table are handled below. Unassigned ECL opcodes return zero in the original.
i32 EnemyState::execute_command(EclContext& context,EclGlobals& globals,EnemyCommandEnvironment& env){
    if(movement_command(context,globals,env.frame))return 0;
    i32 result;if(state_command(context,globals,env.frame,result))return result;
    if(projectile_command(context,globals,env.projectiles))return 0;
    if(animation_command(context,globals,env.animations))return 0;
    if(spawn_command(context,globals,env.spawning))return 0;
    scene_command(context,globals,env.scene);return 0;
}
}
