#include "BattleFrame.hpp"
namespace th075 {
BattleExit advance_battle_frame(BattleFrameState& state,BattleProgress& progress,const std::array<RoundFighterState*,2>& fighters,BattleFrameCallbacks& callbacks){
    callbacks.poll_pause();
    // Camera/HUD/cutscene state advances during the original render callback,
    // before simulation, even when stage geometry presentation is skipped.
    callbacks.render();
    if(!progress.paused&&progress.pauseFade==0){
        ++state.frame;if(state.replayMode==1||state.replayMode==2)++state.replayFrame;
        callbacks.update_inputs();callbacks.frame_events();
        if(progress.rules.freezeFrames>0)--progress.rules.freezeFrames;else callbacks.simulate();
        if(state.draw)callbacks.present();state.draw=state.renderEveryFrame?true:!state.draw;
        return callbacks.advance_state();
    }
    callbacks.pause_menu();
    if(progress.paused){if(progress.pauseFade<128)progress.pauseFade=u8(progress.pauseFade+5);}
    else if(progress.pauseFade)progress.pauseFade=u8(progress.pauseFade-5);
    if(state.draw)callbacks.present();state.draw=state.renderEveryFrame?true:!state.draw;
    if(!progress.paused&&progress.pauseFade==0)for(u32 side=0;side<2;side++)fighters[side]->rawInputs=progress.savedInputs[side];
    return BattleExit::Stay;
}
}
