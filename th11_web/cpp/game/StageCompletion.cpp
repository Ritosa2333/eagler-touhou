#include "StageCompletion.hpp"
#include <algorithm>

namespace th11 {
namespace {
i32 multiply(i32 a,i32 b){return signed_bits(u32(a)*u32(b));}
}
void StageCompletion::count_clear(){
    auto& count=records.clears[mode.selection][economy.difficulty];
    if(count<99999)count=wrapping_add(count,1);
}
bool StageCompletion::complete(){
    if(mode.stage<1||mode.stage>7||mode.selection<0||mode.selection>=6||economy.difficulty<0||economy.difficulty>4)return false;
    if(!effects.stage_result_animation())return false;
    i32 lives=economy.lives;
#ifdef TH11_MULTIPLAYER
    if(multiplayer_count)lives=std::max(lives,0);
#endif
    state.displayed_bonus=multiply(wrapping_add(lives,mode.stage),1000000);
    economy.add_score(state.displayed_bonus);
#ifdef TH11_MULTIPLAYER
    // The shared stage component is already present above. Each additional
    // pilot contributes only its remaining lives, in stable seat order.
    for(unsigned seat=1;seat<multiplayer_count;++seat){const auto& pilot=*multiplayer_economies[seat];const i32 bonus=multiply(std::max(pilot.lives,0),1000000);economy.add_score(bonus);state.displayed_bonus=wrapping_add(state.displayed_bonus,bonus);}
#endif
    state.hud_flags|=0x200;
    state.result_timer.set(0,rate);
    effects.recall_player_options();
    if(mode.control_mode==0&&economy.difficulty!=4)
        records.stages[mode.selection][economy.difficulty*6+mode.stage]={1,1};
    // THPrac 41eb9a -> 41ebcc bypasses both early Practice exits.
    if(!mode.all_clear_bonus){
        if(mode.practice){request(StageExit::Results);return true;}
        if(mode.control_mode!=0&&mode.replay_practice){request(StageExit::ReplayEnd);return true;}
    }
    if(mode.stage!=6&&mode.stage!=7){
        request(mode.force_title?StageExit::Title:StageExit::NextStage);
        state.next_stage=mode.stage+1;
        return true;
    }
    state.hud_flags|=0x20;
    auto award_resources=[&](const GameEconomy& pilot){
        i32 lives=pilot.lives,power=pilot.power;
#ifdef TH11_MULTIPLAYER
        if(multiplayer_count){lives=std::max(lives,0);power=std::max(power,0);}
#endif
        const i32 points=pilot.point_value/100;
        economy.add_score(multiply(points-points%10,1000));
        if(mode.stage==7){
            // Preserve the original per-component wrapping and score cap.
            economy.add_score(multiply(lives,40000000));
            economy.add_score(multiply(power,400000));
            return;
        }
        i32 bonus=0;
        switch(pilot.difficulty){
        case 0:bonus=multiply(wrapping_add(multiply(lives,200),power),100000);break;
        case 1:bonus=wrapping_add(multiply(lives,25000000),multiply(power,150000));break;
        case 2:bonus=multiply(wrapping_add(multiply(lives,175),power),200000);break;
        case 3:bonus=wrapping_add(multiply(lives,40000000),multiply(power,300000));break;
        case 4:bonus=multiply(wrapping_add(multiply(lives,100),power),400000);break;
        }
        economy.add_score(bonus);
        state.displayed_bonus=wrapping_add(state.displayed_bonus,bonus);
    };
    award_resources(economy);
#ifdef TH11_MULTIPLAYER
    for(unsigned seat=1;seat<multiplayer_count;++seat)award_resources(*multiplayer_economies[seat]);
#endif
    if(mode.stage==6){
        // 41eca6/41ed4a re-check Practice after the all-clear award, before
        // entering the ending or incrementing full-game clear records.
        if(mode.all_clear_bonus&&mode.practice){request(StageExit::Results);return true;}
        if(mode.all_clear_bonus&&mode.control_mode!=0&&mode.replay_practice){request(StageExit::ReplayEnd);return true;}
        if(mode.replay_mode==1){request(StageExit::ReplayEnd);return true;}
        state.hud_flags|=0x10;
        state.ending_frames=0;
        count_clear();
    }else{
        if(mode.all_clear_bonus&&mode.practice){request(StageExit::Results);return true;}
        if(mode.all_clear_bonus&&mode.control_mode!=0&&mode.replay_practice){request(StageExit::ReplayEnd);return true;}
        if(mode.replay_mode==1){request(StageExit::ReplayEnd);return true;}
        request(StageExit::Results);
        count_clear();
    }
    return true;
}
void StageCompletion::update(){
    if(state.hud_flags&0x200)state.result_timer.tick();
    if(!(state.hud_flags&0x10))return;
    state.ending_frames=wrapping_add(state.ending_frames,1);
    if(state.ending_frames==180)effects.start_ending_fade();
    if(state.ending_frames>=380){
        request(mode.control_mode!=0?StageExit::ReplayEnd:mode.force_title?StageExit::Title:StageExit::Ending);
    }
}
}
