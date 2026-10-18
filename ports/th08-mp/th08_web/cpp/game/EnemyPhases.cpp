#include "EnemyPopulation.hpp"
#include "GuiState.hpp"
#include "EnemyRetirement.hpp"
namespace th08 {
void EnemyPopulation::cancel_async(EclVm& enemy){
    cancel_enemy_async(enemy);
}
void EnemyPopulation::reset_emitter(EclVm& enemy){
    enemy.rank_speed_low=-.5f;enemy.rank_speed_high=.5f;enemy.rank_count_low=enemy.rank_count_high=enemy.rank_layers_low=enemy.rank_layers_high=0;
    enemy.scratch_depth=0;enemy.flags2&=~0x30u;enemy.emitter=executor.game_state().emitter_template;enemy.emission_period=0;
}
void EnemyPopulation::kill_non_bosses(){
    for(u32 index=0;index<480;++index)if(auto* enemy=enemies[index].get();enemy&&(enemy->flags&1)&&!(enemy->flags&2)){
        enemy->life=0;if(enemy->death_subroutine>=0){executor.switch_main(*enemy,enemy->death_subroutine);enemy->death_subroutine=-1;}
    }
}
void EnemyPopulation::protect_player(const EclVm& enemy){
    auto& globals=executor.game_state();if((enemy.flags&2)&&globals.player_state==0){globals.player_state_timer.set(70);globals.player_state=3;}
}
bool EnemyPopulation::check_life(EclVm& enemy){
    auto& globals=executor.game_state();enemy.flags2&=~0x30u;u32 thresholds=0;const bool spell=globals.spell_flags&1;
    const auto warn=[&](i32 remaining,i32 high,i32 middle,i32 low){const u32 tier=remaining<low?3:remaining<middle?2:remaining<high?1:0;if(((enemy.flags2>>4)&3)<tier)enemy.flags2=(enemy.flags2&~0x30u)|(tier<<4);};
    for(u32 index=0;index<4;++index)if(enemy.life_thresholds[index]>=0){
        ++thresholds;const i32 threshold=enemy.life_thresholds[index];
        if(enemy.life<threshold){
            enemy.life=enemy.remaining_life=threshold;if(!executor.switch_main(enemy,enemy.life_subroutines[index]))return false;
            enemy.life_thresholds[index]=-1;enemy.remaining_seconds=wrapping_sub(enemy.timeout,enemy.lifetime.current)/60;enemy.timeout=-1;
            cancel_async(enemy);reset_emitter(enemy);
            if(globals.phase_actions)globals.phase_actions->clear_familiars(enemy,true);
            kill_non_bosses();protect_player(enemy);return true;
        }
        const i32 remaining=wrapping_sub(enemy.life,threshold);if(spell)warn(remaining,300,200,120);else warn(remaining,2200,1500,500);
    }
    if(!thresholds){if(enemy.flags&2){if(spell)warn(enemy.life,400,300,120);else warn(enemy.life,2400,1600,600);}else warn(enemy.life,0,0,spell?10:50);}
    return false;
}
bool EnemyPopulation::check_timeout(EclVm& enemy){
    auto& globals=executor.game_state();
    if((enemy.flags&2)&&u8(enemy.boss_id)==0&&globals.gui)globals.gui->spell_seconds=wrapping_sub(enemy.timeout,enemy.lifetime.current)/60;
    if(enemy.lifetime.current<enemy.timeout)return false;
    enemy.remaining_seconds=0;i32 highest=0;u32 selected=0;
    for(u32 index=0;index<4;++index)if(enemy.life_thresholds[index]>=0&&highest<enemy.life_thresholds[index]){highest=enemy.life_thresholds[index];selected=index;}
    if(highest>0){enemy.life=enemy.remaining_life=highest;enemy.life_thresholds[selected]=-1;}
    if(!executor.switch_main(enemy,enemy.timeout_subroutine))return false;
    enemy.timeout=-1;enemy.timeout_subroutine=enemy.death_subroutine;enemy.lifetime.set(0);
    if(!(enemy.flags&0x8000000)){globals.spell_flags=(globals.spell_flags&~4u)|8;globals.spell_capture_bonus=0;if(globals.bullet_actions)globals.bullet_actions->clear(4);}
    protect_player(enemy);if(globals.phase_actions)globals.phase_actions->clear_familiars(enemy,false);
    kill_non_bosses();cancel_async(enemy);reset_emitter(enemy);return true;
}
}
