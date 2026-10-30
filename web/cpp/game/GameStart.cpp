#include "GameStart.hpp"
namespace th17 {
bool GameStart::check(bool ok,const char* message){if(!ok&&error.empty())error=message;return ok;}
bool GameStart::begin(bool fresh){
    if(u32(run.character)>=3||u32(run.animal)>=3||u32(run.difficulty)>=5||run.stage<1||run.stage>7)return check(false,"Invalid run selection");
    const u32 loadout=u32(run.character*3+run.animal);
    if(!run.practice_record)records.title.stage_available[loadout][run.difficulty][run.stage-1]=1;
    if(!fresh){if(u32(score.score)>u32(run.scoring.high_score))run.scoring.high_score=score.score;return true;}
    if(run.stage==7&&run.difficulty<4)run.difficulty=4;score.difficulty=run.difficulty;
    const u32 mode=run.scoring.mode_flags>>4&3;
    if(mode==2){if(u32(run.spell_id)>=101)return check(false,"Invalid spell practice selection");run.scoring.high_score=records.spell_best[loadout][run.spell_id];run.scoring.high_score_extra=0;}
    else if(mode==0){const auto& high=records.high_scores[loadout][run.difficulty];run.scoring.high_score=high.score;run.scoring.high_score_extra=i8(high.extra);}
    else{run.scoring.high_score=records.practice_scores[loadout][run.difficulty][run.stage];run.scoring.high_score_extra=0;}
    if(!(run.scoring.mode_flags&8))run.scoring.continues=0;
    grazes=score.score=score.point_items=player.power=0;
    player.bombs=run.animal==1?4:3;player.power_step=1;player.initial_bombs=player.bombs;
    if(!check(host.previous_bomb_display(player.bombs,player.bomb_pieces),"Previous bomb display unavailable"))return false;
    player.bomb_pieces=player.life_pieces=score.life_piece_tier=player.deaths=0;
    roaring.count=0;roaring.reward_age.set(0);roaring.countdown.set(0);roaring.duration=roaring.kind=roaring.extension=0;
    run.initial_point_value=1000000;score.max_point_value=50000000;
    score.point_value=truncate_int(float(float(run.initial_point_value)/100.f)*100.f);
    run.ranking_default=run.difficulty==4?0:5;
    if(mode==2){player.extra_lives=0;player.bombs=0;if(!check(host.previous_bomb_display(0,0),"Spell practice bomb display unavailable"))return false;}
    else if(mode==0)player.extra_lives=2;
    else player.extra_lives=run.practice_lives?wrapping_sub(run.practice_lives,1):9;
    if(!check(host.create_player(),"Player creation failed"))return false;
    const i32 full=wrapping_mul(player.power_step,4);
    if(mode==2||run.stage==7)player.power=full<=score.max_power?std::max(full,player.power_step):score.max_power;
    else if(run.stage<2)player.power=std::min(player.power_step,score.max_power);
    else player.power=full<=score.max_power?std::max(full,player.power_step):score.max_power;
    if(!check(host.configure_player(),"Initial player options failed"))return false;
    run.scoring.mode_flags&=~4u;
    if(!run.practice_record&&run.demo_slot<0&&records.starts[loadout]<9999999)records.starts[loadout]=wrapping_add(records.starts[loadout],1);
    return true;
}
}
