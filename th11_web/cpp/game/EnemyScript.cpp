#include "EnemyScript.hpp"
#include "EnemyAnimations.hpp"
#include "EnemyManager.hpp"
#include "SpellController.hpp"
#include <cstdlib>
namespace th11 {
i32 EnemyScriptServices::update(float elapsed){if(error.instruction)return -2;
#ifdef TH_ENABLE_THPRAC
    const int stage=commands.spells?commands.spells->stage:0;
    if(commands.practice_time_lock&&stage>=4&&stage<=6&&globals.enemy.flags==20512&&commands.bosses[0]){
        elapsed=0;constexpr float waits[]{-900,-1800,-1000};
        for(auto* node=&globals.enemy.script_owner->script.threads;node;node=node->next)if(node->value->time==waits[stage-4]||(stage==4&&node->value->time==-600))node->value->time=-100;
    }
#endif
    return globals.enemy.script_owner->script.update_threads(elapsed,*this);}
i32 EnemyScriptServices::command(EclContext& context){
    auto& enemy=globals.enemy;
    if(enemy_movement_command(enemy,context,globals)||enemy_emitter_command(enemy,context,globals))return 0;
    i32 result=-2;
    const u16 op=context.instruction->opcode;
    if(commands.manager&&((op>=0x100&&op<=0x10f&&op!=0x102&&op!=0x103&&op!=0x106&&op!=0x107&&op!=0x108&&op!=0x10d)||op==0x149||op==0x150||op==0x159||op==0x173||op==0x1c2))result=commands.manager->command(enemy,context,globals);
    else if(op==0x191)result=enemy_fire_command(enemy,context,globals,commands);
    else if(op==0x16d||op==0x19a||(op>=0x19c&&op<=0x1a5)||(op>=0x1ac&&op<=0x1ad)||(op>=0x1af&&op<=0x1b2)||(op>=0x1be&&op<=0x1c0))result=enemy_laser_command(enemy,context,globals,commands);
    else if(context.instruction->opcode>=0x102&&context.instruction->opcode<=0x115&&commands.animations)result=commands.animations->command(enemy,context,globals);
    else result=enemy_state_command(enemy,context,globals,commands);
    if(result==-2)error={context.instruction,context.thread_id,context.time,context.instruction->opcode};
    return result;
}
void* EnemyScriptServices::allocate(u32 bytes){return std::calloc(1,bytes);}
void EnemyScriptServices::release(void* p){std::free(p);}
}
