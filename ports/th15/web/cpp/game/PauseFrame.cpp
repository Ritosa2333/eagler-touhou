#include "PauseFrame.hpp"
namespace th15 {
PauseFrame::PauseFrame(PauseState& s,SessionState& p,PlayerLifeSession& v,Timer& age,PauseActivation& a,PauseMenu& m,FrameScheduler& f,i32 bank):state(s),progress(p),player(v),session_age(age),activation(a),menu(m),scheduler(f),front(bank){callback.owner=this;callback.run=[](void* p){return static_cast<PauseFrame*>(p)->scheduled();};callback.enabled=true;if(scheduler.add(callback,FramePass::Update,10)<0)fail("Pause callback registration failed");}
PauseFrame::~PauseFrame(){scheduler.remove(callback);}
bool PauseFrame::fail(const std::string& why){if(error.empty())error=why.empty()?"Pause frame failed":why;return false;}
i32 PauseFrame::scheduled(){return update(input)?i32(FrameAction::Continue):i32(FrameAction::Error);}
bool PauseFrame::update(const PauseFrameInput& controls){
 if(!error.empty())return false;
 if(state.screen==PauseScreen::Inactive){
  if(!(player.mode_flags&0x40)&&!(progress.scene_flags&0x10000)&&((controls.pressed&0x100)||controls.lost_focus)&&controls.session_enabled&&session_age.current>29)
   if(!activation.open(PauseEntrance::Pause,front))return fail(activation.error);
 }else if(state.screen==PauseScreen::Pause||state.screen==PauseScreen::GameOver||state.screen==PauseScreen::Results){if(!menu.update(controls.pressed,controls.repeated))return fail(menu.error);}
 state.age.tick(&progress.rate);state.selection_age.tick(&progress.rate);return true;
}
}
