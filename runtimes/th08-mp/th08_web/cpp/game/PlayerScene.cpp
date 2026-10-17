#include "PlayerScene.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/WorldJournal.hpp"
#endif
namespace th08 {
void PlayerScene::before_shot_write(PlayerShot& shot){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(world&&world->world_journal)failed|=!world->world_journal->TouchShot(shot);
#else
    (void)shot;
#endif
}
void PlayerScene::Patterns::before_objects_write(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(s.world&&s.world->world_journal)s.failed|=!s.world->world_journal->TouchBomb(s.state.bomb_objects);
#endif
}
void PlayerScene::sync_values(){auto& c=state.context;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    c.challenge_mode=numbers.pilot.challenge_mode;
    if(c.challenge_mode)numbers.bombs=0;
#endif
c.bombs=Scalar::truncate(numbers.bombs);c.lives=Scalar::truncate(numbers.lives);c.power=Scalar::truncate(numbers.power);c.time_orbs=numbers.time_orbs;c.last_spell_requirement=numbers.last_spell_requirement;c.gauge=numbers.gauge;}
void PlayerScene::Patterns::team_invincibility(i32 frames){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(!s.world||!s.world->roster)return;
    auto& roster=*s.world->roster;
    for(u32 seat=0;seat<roster.count;++seat){
        if(!roster.eligible(seat)||i32(seat)==s.seat)continue;
        auto& life=roster.seats[seat].player->status().life;
        if(life.team_bomb_protection.current<frames)life.team_bomb_protection.set(frames);
        if(life.state!=0&&life.state!=3)continue;
        const i32 remaining=life.state==3?life.timer.current:0;
        life.state=3;if(remaining<frames)life.timer.set(frames);
    }
#else
    (void)frames;
#endif
}
void PlayerScene::Patterns::spell_overlay(i32 form,const char* name,i32 style){
    auto& p=s.world->announcement;p.context.game_flags=s.state.context.game_flags;p.context.current_spell=s.world->ecl.current_spell;s.failed|=!p.player(form,name,style);
}
bool PlayerScene::prepare(){
    if(!world){failed=true;return false;}sync_values();state.context.game_flags=world->ecl.game_flags;state.context.pause=world->ecl.paused;state.context.time_spell=u8(world->ecl.spell_flags&1);
#if !defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
    state.context.game_over=world->ecl.stage_completion;
#endif
    std::memcpy(&state.context.hud_flags,&world->hud.flags,4);state.input.gui_blocked=gui_blocked();
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
    state.input.tampered=false;
#else
    state.input.tampered=values.tampered();
#endif
    state.context.cheats=state.bomb_input.cheats=u8(practice_cheats());
    for(u32 i=0;i<8;i++){auto* enemy=world->ecl.boss_slots[i];boss_owners[i]=enemy;state.bomb_input.bosses[i]=enemy?&boss_views[i]:nullptr;if(enemy)boss_views[i]={enemy->life,enemy->flags};}
    return !failed;
}
void PlayerScene::finish(){
    for(u32 i=0;i<8;i++)if(auto* enemy=boss_owners[i]){enemy->life=boss_views[i].life;enemy->flags=boss_views[i].flags;}
    std::memcpy(&world->hud.flags,&state.context.hud_flags,4);world->ecl.game_flags=state.context.game_flags;world->ecl.paused=u8(state.context.pause);
#if !defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
    world->ecl.player_state=state.life.state;world->ecl.player_state_timer=state.life.timer;world->ecl.player=state.motion.movement.position;
    world->ecl.stage_completion=state.context.game_over;
#endif
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
