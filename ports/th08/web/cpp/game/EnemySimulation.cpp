#include "EnemySimulation.hpp"
#include "EnemyVisibility.hpp"
#include "EnemyRetirement.hpp"
#include "GuiState.hpp"
#include "AsciiManager.hpp"
namespace th08 {
EnemySimulation::EnemySimulation(EnemySimulationState& s,EnemySimulationInput& i,EnemyPopulation& p,EclProgram& file,EclExecutor& e,const FrameTiming& t,Rng& rng,GameGlobals& n,GameValues& v,GameRank& r,GameGauge& g,DamageRegions& regions,PlayerFrameState& player,EnemySimulationActions& a)
    :state(s),input(i),population(p),program(file),executor(e),timing(t),globals(e.game_state()),numbers(n),values(v),rank(r),player(player),actions(a),timeline_executor(rng,t,globals),familiars(i.familiar,g,rng,s.drops,regions,a),death(e,familiars,i.familiar,s.drops,rng,g,v,a,*this,p.replay_flags){bind();}
void EnemySimulation::bind()noexcept{globals.phase_actions=this;globals.enemy_actions=this;globals.timeline_actions=this;globals.player_frame=&player;globals.rank_value=&rank.value;globals.frame_count_value=&state.frames;}
EnemySimulation::~EnemySimulation(){if(globals.phase_actions==this)globals.phase_actions=nullptr;if(globals.enemy_actions==this)globals.enemy_actions=nullptr;if(globals.timeline_actions==this)globals.timeline_actions=nullptr;if(globals.rank_value==&rank.value)globals.rank_value=nullptr;if(globals.frame_count_value==&state.frames)globals.frame_count_value=nullptr;}
void EnemySimulation::message(i32 entry){actions.message(entry);globals.dialogue_active=dialogue_active();}
bool EnemySimulation::dialogue_active()const noexcept{
    const auto* gui=globals.gui?globals.gui->implementation:nullptr;return gui&&(gui->dialogue.message>=0||gui->dialogue.message==-2);
}
void EnemySimulation::sync_familiars()noexcept{input.familiar.time_spell=u8(globals.spell_flags&1);input.familiar.boss_present=0;for(const auto* boss:globals.boss_slots)if(boss)input.familiar.boss_present=1;}
void EnemySimulation::clear_familiars(EclVm& enemy,bool reward){sync_familiars();failed|=!familiars.clear(enemy,reward);}
EnemySpawnResult EnemySimulation::spawn(const TimelineSpawn& request,const EclContext::Locals& locals){auto* enemy=population.spawn_inherited(request,locals);failed|=enemy->invalid;return {enemy,population.spawn_failed};}
void EnemySimulation::spawn(const TimelineSpawn& request){auto* enemy=population.spawn(request);failed|=enemy->invalid;}
i32 EnemySimulation::cancel_projectiles(i32 maximum,bool reward){i32 score=0;failed|=!actions.cancel_projectiles(maximum,reward,score);return score;}
i32 EnemySimulation::cancel_enemies(i32 maximum,i32 score){failed|=!population.cancel_for_score(maximum,score,actions);return score;}
bool EnemySimulation::finish_enemy(EclVm& enemy,bool hit){
    update_enemy_hit_flash(enemy,hit,actions);
    if(enemy.flags&2){
        const u32 boss=u8(enemy.boss_id);
        if(!dialogue_active()&&boss==0&&globals.gui)globals.gui->boss_life_max=(Extended::from_int(enemy.life)/Extended::from_int(enemy.initial_life)).to_float();
        if(globals.ascii){
            if(boss>=4){enemy.invalid=true;return false;}
            globals.ascii->boss_markers[boss].pos={(enemy.flags&16)?-999:Scalar::add(enemy.resolved_position.x,32),472,0};
            const u32 warning=(enemy.flags2>>4)&3;globals.ascii->boss_states[boss]=warning?warning+1:enemy.animation[0].flag17;
        }
    }
    enemy.update_attached_effects();if(enemy.invalid)return false;
    if(!globals.paused)enemy.lifetime.tick(timing);
    if(enemy.damage_protection.current>0)enemy.damage_protection.decrement(1,timing);
    if(!(enemy.flags&16)&&(enemy.flags&1)){
        if(enemy.draw_layer>=4){enemy.invalid=true;return false;}
        enemy.next_in_layer=state.layers[enemy.draw_layer];state.layers[enemy.draw_layer]=&enemy;
    }
    return true;
}
JobResult EnemySimulation::update(){
    failed=false;i32 bomb_hit=0;
    if(!dialogue_active()){
        state.frames=wrapping_add(state.frames,1);
        if(state.timer.current>=16){state.active_frames=wrapping_add(state.active_frames,1);if(!input.focused)state.unfocused_frames=wrapping_add(state.unfocused_frames,1);}
    }
    if(globals.game_flags&0x400)return JobResult::Continue;
    if((globals.game_flags&0x2000)&&globals.boss_slots[0])actions.damage({192,224,0},{384,448,0},globals.boss_slots[0]->time_items,bomb_hit);
    if(!dialogue_active()&&state.timer.changed()){
        const i32 lives=Scalar::truncate(numbers.lives),interval=wrapping_sub(2400,signed_bits(u32(lives)*240u));
        if(!interval){failed=true;return JobResult::Error;}
        if(i64(state.timer.current)%interval==0)rank.add(100);
    }
    for(auto& layer:state.layers)layer=nullptr;
    for(u32 i=0;i<program.timeline_count();++i){
        auto& timeline=state.timelines[i];if(!timeline.instruction){const Timer clock=timeline.timer;if(!timeline_executor.start(timeline,program,i,globals.difficulty)){failed=true;return JobResult::Error;}timeline.timer=clock;}
        globals.dialogue_active=dialogue_active();timeline_executor.step(timeline);if(timeline.invalid||failed){failed=true;return JobResult::Error;}
    }
    state.active_count=0;
    for(u32 index=0;index<480;++index){
        auto* enemy=population.at(index);if(!enemy)continue;
        if(!(enemy->flags&1)){if(player.target_reference==enemy)player.target_reference=nullptr;continue;}
        bool hit=false,die=false;
        if(enemy->flags&0x400){enemy->refresh_position();enemy->resolved_position.z=0;die=true;}
        else{
            state.active_count=wrapping_add(state.active_count,1);
            const bool frozen=((enemy->flags&0x40000000)&&(input.familiar.bomb||globals.player_state!=0))||(enemy->flags2&0x80);
            if(frozen)enemy->lifetime.decrement(1,timing);
            else{
                if(enemy->flags&0x100)update_enemy_form(*enemy,globals.youkai,actions);
                bool removed=false;u32 transitions=0;
                for(;;){
                    if(!executor.step(*enemy)){enemy->flags&=~1u;sync_familiars();failed|=!retire_enemy(*enemy,globals,population.replay_flags);removed=true;break;}
                    enemy->integrate_position(timing);if(enemy->familiar_effect)actions.move_overlay(*enemy->familiar_effect,enemy->resolved_position);
                    if(!update_enemy_trail(*enemy)){failed=true;return JobResult::Error;}
                    if(!update_enemy_visibility(*enemy)){
                        if(enemy->invalid){failed=true;return JobResult::Error;}sync_familiars();failed|=!retire_enemy(*enemy,globals,population.replay_flags);removed=true;break;
                    }
                    const bool changed=population.check_life(*enemy)||(enemy->timeout>=0&&population.check_timeout(*enemy));
                    if(enemy->invalid||failed){failed=true;return JobResult::Error;}if(!changed)break;
                    if(++transitions>=10000){enemy->invalid=failed=true;return JobResult::Error;}
                }
                if(failed)return JobResult::Error;if(removed)continue;
                if(!executor.update_animations(*enemy)){failed=true;return JobResult::Error;}
                bomb_hit=input.familiar.bomb;
                if(!(enemy->flags&0x830)&&(!(enemy->flags&0x80000000)||!input.familiar.bomb)){
                    if(!contact_enemy_and_trail(*enemy,input.familiar.character,actions)){failed=true;return JobResult::Error;}
                    // ECL may begin/end a spell during this enemy's update.
                    // Damage reduction must observe the current spell flags.
                    const EnemyDamageContext damage{globals.player,input.familiar.character,input.familiar.bomb,u8(globals.spell_flags&1),u8((globals.spell_flags&128)!=0)};
                    hit=damage_enemy(*enemy,damage,player,values,bomb_hit,actions);
                }
                if((enemy->flags2&8)&&enemy->life>0)enemy->flags2&=~8u;
                die=enemy->life<=0&&!(enemy->flags2&0x48);
            }
        }
        if(die){sync_familiars();if(!death.run(*enemy,index,input.focused,bomb_hit)){failed=true;return JobResult::Error;}}
        if(failed||!finish_enemy(*enemy,hit)){failed=true;return JobResult::Error;}
    }
    if(state.timer.current%200==0&&values.tampered())return JobResult::Exit;
    state.timer.tick(timing);return JobResult::Continue;
}
}
