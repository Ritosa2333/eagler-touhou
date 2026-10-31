#pragma once
#include "StageGameplay.hpp"
#include "ReplayRecording.hpp"
namespace th17 {
struct RunInputServices {virtual ~RunInputServices()=default;virtual bool replay_finished()=0;};
// Native priority 17: edges and hold durations belong to this run, while the
// pause callback at 11 can stop the pass before a replay frame is consumed.
class RunInput {
 StageGameplay& scene;GameRun& progress;RunInputServices& host;Replay* replay;ReplayRecording* recording;
 FrameCallback callback,acceleration_callback;ReplayStageState snapshot;bool active=false,entered=false;u32 physical=0;float fps=60;PlayerTouch touch;
 bool fail(const std::string&);i32 scheduled();i32 accelerate_replay();
public:
 GameInput input;bool auto_focus=false;u8 recorded_fps=60;std::string error;
 RunInput(StageGameplay&,GameRun&,RunInputServices&,Replay*,ReplayRecording*,u16 seed,bool fresh);
 ~RunInput();bool activate();void controls(u32 held,float rate,PlayerTouch value={})noexcept{physical=held;fps=rate;touch=value;}
};
}
