#include "EnemyDeath.hpp"
#include "EnemyRetirement.hpp"
#include "GuiState.hpp"
namespace th08 {
bool EnemyDeath::run(EclVm& enemy,i32 slot,bool focused,i32 bomb_hit){
    auto& globals=executor.game_state();enemy.flags2|=8;
    enemy.remaining_seconds=wrapping_sub(enemy.timeout,enemy.lifetime.current)/60;
    enemy.timeout=-1;for(auto& value:enemy.life_thresholds)value=-1;cancel_enemy_async(enemy);
    if(enemy.parent)enemy.parent->summoned_familiars=wrapping_sub(enemy.parent->summoned_familiars,1);
    if(!familiars.clear(enemy,true))return false;
    gauge.add(focused?200:-200,input.bomb,false);
    const auto hide_boss=[&](){globals.gui_blocks_spawn=false;if(globals.gui)globals.gui->boss_present=false;};
    const auto stop_overlay=[&](){if(enemy.familiar_effect){enemy.familiar_effect->pendingInterrupt=3;enemy.familiar_effect=nullptr;}};
    const u32 mode=(enemy.flags>>20)&7;
    if(mode<=2){
        if(mode<2){
            values.add_score(enemy.score_reward);
            if(mode==0){enemy.flags&=~1u;stop_overlay();}
            else enemy.flags=(enemy.flags|0x800000u)&~0x4cu;
            if(enemy.flags&2){hide_boss();if(!retire_enemy_effects(enemy))return false;}
        }
        drop_enemy_items(enemy,bomb_hit!=0,drops,random,visuals);
        if((enemy.flags&2)&&!input.time_spell){
            i32 score=actions.cancel_projectiles(8000,true);score=actions.cancel_enemies(8000,score);
            if(score){values.add_score(score);actions.popup_bonus(score);}
        }
        enemy.life=0;replay|=0x20;
    }else if(mode==3){
        enemy.life=1;enemy.flags&=~0x700008u;hide_boss();replay|=0x20;
        if(i8(enemy.death_effects[0])>=0)for(u32 i=0;i<3;++i)visuals.effect(i8(enemy.death_effects[0]),enemy.resolved_position,1,0xffffffffu);
        stop_overlay();if(globals.player_state==0){globals.player_state_timer.set(90);globals.player_state=3;}
        enemy.flags&=~0xc0000000u;
    }
    if(!(enemy.flags&0x400)){
        visuals.panned_sound(2+slot%2,enemy.resolved_position.x);
        if(i8(enemy.death_effects[0])>=0){
            visuals.effect(i8(enemy.death_effects[0]),enemy.resolved_position,1,0xffffffffu);
            visuals.effect(enemy.death_effects[1]+4,enemy.resolved_position,4,0xffffffffu);
        }
        if(gauge.human_bonus()||gauge.youkai_bonus())visuals.item(enemy.resolved_position,7,1);
    }
    if(enemy.death_subroutine>=0){
        enemy.rank_speed_low=-.5f;enemy.rank_speed_high=.5f;
        enemy.rank_count_low=enemy.rank_count_high=enemy.rank_layers_low=enemy.rank_layers_high=0;
        // Original writes the working depth, preserving the saved main depth.
        enemy.scratch_depth=0;for(auto& value:enemy.life_thresholds)value=-1;enemy.timeout=-1;cancel_enemy_async(enemy);
        enemy.emitter=globals.emitter_template;enemy.emission_period=0;
        if(!executor.switch_main(enemy,enemy.death_subroutine))return false;enemy.death_subroutine=-1;
    }
    return !enemy.invalid;
}
}
