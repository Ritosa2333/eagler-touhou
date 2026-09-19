#include "PlayerScene.hpp"
namespace th08 {
void PlayerScene::sync_values(){auto& c=state.context;c.bombs=Scalar::truncate(numbers.bombs);c.lives=Scalar::truncate(numbers.lives);c.power=Scalar::truncate(numbers.power);c.time_orbs=numbers.time_orbs;c.last_spell_requirement=numbers.last_spell_requirement;c.gauge=numbers.gauge;}
void PlayerScene::Patterns::spell_overlay(i32 form,const char* name,i32 style){
    auto& p=s.world->announcement;p.context.game_flags=s.state.context.game_flags;p.context.current_spell=s.world->ecl.current_spell;s.failed|=!p.player(form,name,style);
}
bool PlayerScene::prepare(){
    if(!world){failed=true;return false;}sync_values();state.context.game_flags=world->ecl.game_flags;state.context.pause=world->ecl.paused;state.context.time_spell=u8(world->ecl.spell_flags&1);state.context.game_over=world->ecl.stage_completion;
    std::memcpy(&state.context.hud_flags,&world->hud.flags,4);state.input.gui_blocked=gui_blocked();state.input.tampered=values.tampered();
    state.context.cheats=state.bomb_input.cheats=u8(practice_cheats());
    for(u32 i=0;i<8;i++){auto* enemy=world->ecl.boss_slots[i];boss_owners[i]=enemy;state.bomb_input.bosses[i]=enemy?&boss_views[i]:nullptr;if(enemy)boss_views[i]={enemy->life,enemy->flags};}
    return !failed;
}
void PlayerScene::finish(){
    for(u32 i=0;i<8;i++)if(auto* enemy=boss_owners[i]){enemy->life=boss_views[i].life;enemy->flags=boss_views[i].flags;}
    std::memcpy(&world->hud.flags,&state.context.hud_flags,4);world->ecl.game_flags=state.context.game_flags;world->ecl.paused=u8(state.context.pause);world->ecl.player_state=state.life.state;world->ecl.player_state_timer=state.life.timer;world->ecl.player=state.motion.movement.position;
    world->ecl.stage_completion=state.context.game_over;
}
void PlayerScene::defeat_boss(u32 slot){
    if(slot>=8){failed=true;return;}auto& globals=world->ecl;
    if(auto* enemy=globals.boss_slots[slot]){
        // Original 0044c77f clears familiars without rewards before the
        // automatic Last Spell termination sends the boss through death.
        if(!globals.phase_actions){failed=true;return;}
        globals.phase_actions->clear_familiars(*enemy,false);failed|=enemy->invalid;
        enemy->life=0;enemy->flags&=~0x40000000u;boss_views[slot]={enemy->life,enemy->flags};
    }
}
}
