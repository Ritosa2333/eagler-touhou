#pragma once
#include "BattleProgress.hpp"
namespace th075 {
struct BattleFrameState {u32 frame=0,replayFrame=0;u8 replayMode=0;bool draw=true,renderEveryFrame=true;};
struct BattleFrameCallbacks {
    virtual ~BattleFrameCallbacks()=default;
    virtual void poll_pause()=0;virtual void render()=0;
    virtual void update_inputs()=0;virtual void frame_events()=0;virtual void simulate()=0;
    virtual void pause_menu()=0;virtual void present()=0;virtual BattleExit advance_state()=0;
};
BattleExit advance_battle_frame(BattleFrameState&,BattleProgress&,const std::array<RoundFighterState*,2>&,BattleFrameCallbacks&);
}
