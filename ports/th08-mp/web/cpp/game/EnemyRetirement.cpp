#include "EnemyRetirement.hpp"
#include "GuiState.hpp"
#include "AsciiManager.hpp"
#include "EffectState.hpp"
#include "PlayerFrame.hpp"
namespace th08 {
void cancel_enemy_async(EclVm& enemy)noexcept{for(u32 i=0;i<4;++i){enemy.asynchronous[i].reset();++enemy.asynchronous_generations[i];}}
bool retire_enemy_effects(EclVm& enemy)noexcept{
    if(enemy.effect_count>24){enemy.invalid=true;return false;}
    for(i32 i=0;i<enemy.effect_count;++i)if(enemy.effects[i]){enemy.effects[i]->dying=1;enemy.effects[i]=nullptr;}
    enemy.effect_count=0;return true;
}
bool retire_enemy(EclVm& enemy,EclGlobals& globals,u16& replay_flags){
    if(globals.phase_actions)globals.phase_actions->clear_familiars(enemy,false);
    else if(enemy.parent||enemy.next_familiar){enemy.invalid=true;return false;}
    if(enemy.invalid)return false;
    if(!((enemy.flags>>20)&7))enemy.flags&=~1u;
    const u32 boss=u8(enemy.boss_id);
    if((enemy.flags&2)&&boss<4){
        globals.gui_blocks_spawn=false;if(globals.gui)globals.gui->boss_present=false;
        globals.boss_slots[boss]=nullptr;enemy.flags&=~2u;
        if(globals.ascii){globals.ascii->boss_markers[boss].pendingInterrupt=2;globals.ascii->boss_markers[boss].pos={-999,-999,0};}
    }
    if(enemy.effect_count&&!retire_enemy_effects(enemy))return false;
    if(enemy.flags&2){if(boss>=8){enemy.invalid=true;return false;}globals.boss_slots[boss]=nullptr;}
    replay_flags|=0x20;
    if(enemy.familiar_effect){enemy.familiar_effect->pendingInterrupt=3;enemy.familiar_effect=nullptr;}
    for(auto& threshold:enemy.life_thresholds)threshold=-1;enemy.timeout=-1;cancel_enemy_async(enemy);
    if(globals.player_frame&&globals.player_frame->target_reference==&enemy)globals.player_frame->target_reference=nullptr;
    return !enemy.invalid;
}
}
