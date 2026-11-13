#include "StageCompletion.hpp"

namespace th11 {
namespace {
i32 multiply(i32 a,i32 b){return signed_bits(u32(a)*u32(b));}
}
void StageCompletion::count_clear(){
    auto& count=records.clears[mode.selection][economy.difficulty];
    if(count<99999)count=wrapping_add(count,1);
#if PORT_TITLE == 14
    auto& clean=records.no_continue_clears[mode.selection][economy.difficulty];
    if(mode.continues==0&&clean<99999)clean=wrapping_add(clean,1);
#endif
}
bool StageCompletion::complete(){
    if(mode.stage<1||mode.stage>7||mode.selection<0||mode.selection>=6||economy.difficulty<0||economy.difficulty>4)return false;
    if(!effects.stage_result_animation())return false;
#if PORT_TITLE == 14
    // Retail434c10/436f90: stage*3M; final awards use lives and bombs.
    state.displayed_bonus=multiply(mode.stage,3000000);
    economy.add_score(state.displayed_bonus);state.hud_flags|=kStageResultFlag;
    state.result_timer.set(0,rate);effects.recall_player_options();
    if(mode.practice||mode.replay_practice){
        if(mode.control_mode!=0){request(StageExit::ReplayEnd);return true;}
        records.stages[mode.selection][economy.difficulty*8+mode.stage][0]=1;
        request(StageExit::Results);return true;
    }
    if(mode.stage!=6&&mode.stage!=7){
        if(mode.control_mode==0)records.stages[mode.selection][economy.difficulty*8+mode.stage][0]=1;
        request(mode.force_title?StageExit::Title:StageExit::NextStage);state.next_stage=mode.stage+1;return true;
    }
#if defined(TH14_CPP_STAGE_DECODE)
    //4370a7/+98 precede HUD+17c and the final score award, not just the
    //later GameSession ending/menu transition.
    if(mode.stage==6){effects.start_ending_control();state.ending_active=true;state.ending_frames=0;}
#endif
    const i32 bonus=mode.stage==6?(economy.difficulty<4?wrapping_add(multiply(economy.lives,10000000),multiply(economy.th14_bombs,3000000)):0):
        multiply(wrapping_add(multiply(economy.lives,10),economy.th14_bombs),4000000);
    economy.add_score(bonus);state.displayed_bonus=wrapping_add(state.displayed_bonus,bonus);
    state.hud_flags|=0x10;
#if !defined(TH14_CPP_STAGE_DECODE)
    if(mode.stage==6){state.ending_active=true;state.ending_frames=0;}
#endif
    if(mode.control_mode!=0){request(StageExit::ReplayEnd);return true;}
    if(mode.stage!=6)request(StageExit::Results);
    count_clear();return true;
#else
    state.displayed_bonus=multiply(wrapping_add(economy.lives,mode.stage),1000000);
    economy.add_score(state.displayed_bonus);
    state.hud_flags|=0x200;
    state.result_timer.set(0,rate);
    effects.recall_player_options();
    if(mode.control_mode==0&&economy.difficulty!=4)
        records.stages[mode.selection][economy.difficulty*(kPortTitle==14?8:6)+mode.stage]={1,1};
    if(mode.practice){request(StageExit::Results);return true;}
    if(mode.control_mode!=0&&mode.replay_practice){request(StageExit::ReplayEnd);return true;}
    if(mode.stage!=6&&mode.stage!=7){
        request(mode.force_title?StageExit::Title:StageExit::NextStage);
        state.next_stage=mode.stage+1;
        return true;
    }
    state.hud_flags|=0x20;
    const i32 points=economy.point_value/100;
    economy.add_score(multiply(points-points%10,1000));
    if(mode.stage==6){
        i32 bonus=0;
        switch(economy.difficulty){
        case 0:bonus=multiply(wrapping_add(multiply(economy.lives,200),economy.power),100000);break;
        case 1:bonus=wrapping_add(multiply(economy.lives,25000000),multiply(economy.power,150000));break;
        case 2:bonus=multiply(wrapping_add(multiply(economy.lives,175),economy.power),200000);break;
        case 3:bonus=wrapping_add(multiply(economy.lives,40000000),multiply(economy.power,300000));break;
        case 4:bonus=multiply(wrapping_add(multiply(economy.lives,100),economy.power),400000);break;
        }
        economy.add_score(bonus);
        state.displayed_bonus=wrapping_add(state.displayed_bonus,bonus);
        if(mode.replay_mode==1){request(StageExit::ReplayEnd);return true;}
        state.hud_flags|=0x10;
        state.ending_frames=0;
        count_clear();
    }else{
        // Separate score calls are observable when the score cap is reached
        // or an original signed 32-bit bonus overflows.
        economy.add_score(multiply(economy.lives,40000000));
        economy.add_score(multiply(economy.power,400000));
        if(mode.replay_mode==1){request(StageExit::ReplayEnd);return true;}
        request(StageExit::Results);
        count_clear();
    }
    return true;
#endif
}
void StageCompletion::update(){
    if(state.hud_flags&kStageResultFlag)state.result_timer.tick();
#if PORT_TITLE == 14
    if(!state.ending_active)return;
#else
    if(!(state.hud_flags&0x10))return;
#endif
    state.ending_frames=wrapping_add(state.ending_frames,1);
    if(state.ending_frames==180)effects.start_ending_fade();
    if(state.ending_frames>=380){
        request(mode.control_mode!=0?StageExit::ReplayEnd:mode.force_title?StageExit::Title:StageExit::Ending);
    }
}
}
