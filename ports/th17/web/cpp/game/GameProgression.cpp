#include "GameProgression.hpp"
namespace th17 {
bool GameProgression::check(bool ok,const char* message){if(!ok&&error.empty())error=message;return ok;}
void GameProgression::add_bonus(i32 value)noexcept{score.score=wrapping_add(score.score,value/10);if(score.score>999999999)score.score=999999999;}
void GameProgression::count_clear(i32 loadout)noexcept{auto& total=records.clears[loadout][run.difficulty];if(total<99999)total=wrapping_add(total,1);if(!run.scoring.continues){auto& clear=records.title.cleared[loadout][run.difficulty];if(clear<99999)clear=wrapping_add(clear,1);}}
bool GameProgression::next_stage(){
    if(!check(StageConfiguration::get(run.stage)&&u32(run.character)<3&&u32(run.animal)<3&&u32(run.difficulty)<5,"Invalid run transition state"))return false;
    const u32 mode=(run.scoring.mode_flags>>4)&3;const i32 loadout=run.character*3+run.animal;
    if(mode!=2){const i32 bonus=wrapping_mul(run.stage,1000000);if(!check(hud.begin_stage_clear(bonus),"Stage clear HUD unavailable"))return false;add_bonus(bonus);}
    if(!check(host.finish_player_options(),"Stage player transition unavailable"))return false;
    if(player.bomb_state&&!check(host.stop_bomb(),"Stage bomb transition unavailable"))return false;
    if(mode){
        if(run.practice_record)return check(host.return_to_title(),"Practice return unavailable");
        if(mode==2){if(!check(u32(run.spell_id)<101,"Invalid spell practice record"))return false;auto& best=records.spell_best[loadout][run.spell_id];const i32 value=wrapping_mul(score.score/10,10);if(best<value)best=value;}
        else records.stage_cleared[loadout][run.difficulty][run.stage]=1;
        return check(host.finish_practice(),"Practice completion unavailable");
    }
    if(run.stage==6){
        run.scene_flags|=0x4000;run.ending_age=0;hud.flags|=0x10;
        if(!check(host.record_play_time(),"Run play time unavailable")||!check(host.trophy(run.scoring.continues?run.character+9:loadout),"Clear trophy unavailable")||!check(host.trophy(run.difficulty+21),"Difficulty trophy unavailable"))return false;
        if(!run.scoring.continues&&!check(host.trophy(run.difficulty+25),"No-continue trophy unavailable"))return false;
        if(!player.deaths){if(!check(host.trophy(29),"No-miss trophy unavailable"))return false;if(run.difficulty==3&&!check(host.trophy(30),"Lunatic no-miss trophy unavailable"))return false;}
        const i32 bonus=wrapping_mul(wrapping_add(player.bombs,wrapping_mul(player.extra_lives,5)),1000000);add_bonus(bonus);hud.stage_clear_bonus=wrapping_add(hud.stage_clear_bonus,bonus);
        if(run.practice_record)return check(host.return_to_title(),"Clear return unavailable");count_clear(loadout);return true;
    }
    if(run.stage==7){
        hud.flags|=0x10;if(run.demo_slot>=0){run.demo_slot=-1;run.screen=(run.application_flags&0x2000)?2:4;return true;}
        const i32 bonus=wrapping_mul(wrapping_add(player.bombs,wrapping_mul(player.extra_lives,5)),1000000);add_bonus(bonus);hud.stage_clear_bonus=wrapping_add(hud.stage_clear_bonus,bonus);
        if(!check(host.trophy(run.animal+(run.character+4)*3),"Extra clear trophy unavailable"))return false;if(!player.deaths&&!check(host.trophy(31),"Extra no-miss trophy unavailable"))return false;
        if(!run.practice_record)count_clear(loadout);else if(!check(host.return_to_title(),"Extra return unavailable"))return false;
        if(!check(host.record_play_time(),"Extra play time unavailable"))return false;run.scene_flags|=0x4000;run.ending_age=0;return true;
    }
    if(!run.practice_record)records.stage_cleared[loadout][run.difficulty][run.stage]=1;
    run.screen=12;if(run.stage<7)run.stage++;return true;
}
}
