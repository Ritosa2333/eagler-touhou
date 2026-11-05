#pragma once
#include "PauseActivation.hpp"
#include "PauseMenu.hpp"
#include "FrameScheduler.hpp"
namespace th15 {
struct PauseFrameInput {u32 pressed=0,repeated=0;bool lost_focus=false,session_enabled=true;};
class PauseFrame {
 PauseState& state;SessionState& progress;PlayerLifeSession& player;Timer& session_age;PauseActivation& activation;PauseMenu& menu;FrameScheduler& scheduler;FrameCallback callback;PauseFrameInput input;i32 front;
 i32 scheduled();bool fail(const std::string&);
public:
 std::string error;
 PauseFrame(PauseState&,SessionState&,PlayerLifeSession&,Timer&,PauseActivation&,PauseMenu&,FrameScheduler&,i32 front);
 ~PauseFrame();
 void controls(const PauseFrameInput& value)noexcept{input=value;}
 void enable(bool value)noexcept{callback.enabled=value;}
 bool update(const PauseFrameInput&);
};
}
