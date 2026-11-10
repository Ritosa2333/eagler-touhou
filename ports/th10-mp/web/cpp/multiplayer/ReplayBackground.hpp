#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Replay background correction belongs to the MP runtime
#endif
#include "../game/Stage.hpp"
namespace th10::multiplayer {
inline bool AlignStage4ReplayCamera(Stage& stage,bool fresh_story_chapter){
    if(!fresh_story_chapter||stage.stage_number!=4||stage.script_timer.current)return false;
    stage.script_timer.current=stage.position_interpolation.timer.current=29;
    stage.script_timer.fractional=stage.position_interpolation.timer.fractional=29.f;
    return true;
}
}
