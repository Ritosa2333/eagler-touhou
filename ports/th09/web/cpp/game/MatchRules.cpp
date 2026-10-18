#include "MatchRules.hpp"
#include <algorithm>
namespace th09 {
void ScoreCounter::advance()noexcept{
    points=std::min(points,999999999u);
    if(displayed==points)return;
    if(points<displayed)points=displayed;
    u32 delta=(points-displayed)/15;delta=std::clamp(delta,1u,0x8d55eu);
    if(increment<delta)increment=delta;
    if(points<increment+displayed)increment=points-displayed;
    displayed+=increment;
    if(points<=displayed){increment=0;displayed=points;}
}
bool ScoreCounter::eligible_for_extend(GameMode mode)const noexcept{
    if(mode==GameMode::story)return displayed>=u32(extends)*2000000u+1000000u;
    if(mode==GameMode::extra)return displayed>=(u32(extends)+1)*500000u;
    return false;
}
bool ScoreCounter::extend(GameMode mode)noexcept{
    bool gained=lives<7;if(gained)lives+=1;
    extends=wrapping_add(extends,1);if(extends>=(mode==GameMode::extra?8:5))extends=99999;
    return gained;
}
void MatchRules::initialize(i32 difficulty,GameMode selected,i32 stage){
    world.difficulty=difficulty;mode=selected;progress=MatchProgress{};progress.stage=stage;
    const bool versus=mode==GameMode::versus;i32 level=1;
    switch(difficulty){
    case 0:world.rank=versus?1:stage/3+1;progress.rank_interval=1800;progress.maximum_rank=versus?16:stage/2+6;break;
    case 1:world.rank=versus?1:stage+1;progress.rank_interval=versus?900:1200;progress.maximum_rank=versus?20:stage+10;break;
    case 2:world.rank=versus?6:stage+3;level=4;progress.rank_interval=versus?720:900;progress.maximum_rank=versus?22:stage/2+15;break;
    case 3:world.rank=versus?8:stage+6;level=8;progress.rank_interval=versus?900:720;progress.maximum_rank=versus?22:stage/2+17;break;
    case 4:world.rank=18;level=10;progress.rank_interval=600;progress.maximum_rank=22;break;
    }
    for(auto& player:attack_levels)for(auto& a:player)a=level;
}
void MatchRules::restart_round(i32 round){
    progress.round=round;progress.round_frames=0;progress.reward_stage=2;progress.reward_stage_frames=0;progress.total_frames=0;progress.active_frames=0;
    if(mode==GameMode::versus)world.rank=std::max(wrapping_sub(world.rank,8),round);
    else{
        switch(world.difficulty){
        case 0:world.rank=progress.stage/3+1;progress.maximum_rank=progress.stage/2+6;progress.rank_interval=(round+3)*600;break;
        case 1:world.rank=progress.stage+1;progress.maximum_rank=progress.stage+10;progress.rank_interval=(round+2)*600;break;
        case 2:world.rank=progress.stage+3;progress.maximum_rank=progress.stage/2+15;progress.rank_interval=round*600+900;break;
        case 3:world.rank=progress.stage+6;progress.maximum_rank=progress.stage/2+17;progress.rank_interval=round*600+900;break;
        case 4:world.rank=18;progress.maximum_rank=22;progress.rank_interval=(round+1)*600;break;
        }
        world.rank=std::max(wrapping_sub(world.rank,round*2),1);
    }
    progress.reward_accumulator=std::max(wrapping_sub(progress.reward_accumulator,1000),0);
}
void MatchRules::update(bool dialogue,u32 flags,u32 first_field_flags){
    auto& p=progress;
    if(!dialogue){
        p.total_frames=wrapping_add(p.total_frames,1);p.round_frames=wrapping_add(p.round_frames,1);
        if(p.rank_interval>0&&p.round_frames%p.rank_interval==p.rank_interval-1)world.rank=std::min(wrapping_add(world.rank,1),p.maximum_rank);
        p.reward_stage_frames=wrapping_add(p.reward_stage_frames,1);
        if(p.round_frames%3600==3599)for(auto& player:attack_levels)for(auto& level:player)if(level<16)++level;
        if(p.reward_stage<6&&p.reward_stage_frames%720==719)++p.reward_stage;
        if(!(flags&0x1800)&&!(first_field_flags&1)){
            const u32 elapsed=u32(p.round_frames);p.reward_accumulator=wrapping_add(p.reward_accumulator,elapsed<1800?3:elapsed<3600?4:elapsed<5400?6:7);
        }
        if(p.reward_accumulator>10000){
            i32 reward=world.random.next16()%4;if(reward==p.last_reward)reward=(reward+1)%4;
            p.reward_accumulator=wrapping_sub(p.reward_accumulator,10000);p.last_reward=reward;
            actions.reward_enemy(0,reward);actions.reward_enemy(1,reward);actions.play_sound(0x33,0);actions.reward_notification(0);actions.reward_notification(1);
        }
    }
    for(auto& s:scores)s.advance();
    if(scores[0].eligible_for_extend(mode)&&scores[0].extend(mode))actions.play_sound(0x1c,0);
    ++p.active_frames;
}
}
