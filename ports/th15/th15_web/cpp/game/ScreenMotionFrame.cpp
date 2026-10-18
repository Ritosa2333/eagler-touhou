#include "ScreenMotionFrame.hpp"
namespace th15 {
ScreenMotionFrame::ScreenMotionFrame(FrameScheduler& s,Rng& random,ScreenShakeSpec spec,std::function<ScreenShakeContext()> c,std::function<void(Vec2,Vec2)> output):scheduler(s),shake(std::make_unique<ScreenShake>(random,spec)),context(std::move(c)),offsets(std::move(output)){install();}
ScreenMotionFrame::ScreenMotionFrame(FrameScheduler& s,Rng& random,ScreenNudgeSpec spec,std::function<ScreenShakeContext()> c,std::function<void(Vec2,Vec2)> output):scheduler(s),nudge(std::make_unique<ScreenNudge>(random,spec)),context(std::move(c)),offsets(std::move(output)){install();}
void ScreenMotionFrame::install(){callback.owner=this;callback.enabled=true;callback.run=[](void* p)->i32{return static_cast<ScreenMotionFrame*>(p)->update();};callback.cleanup=[](void* p)->i32{static_cast<ScreenMotionFrame*>(p)->retire();return 0;};scheduler.add(callback,FramePass::Update,20);}
ScreenMotionFrame::~ScreenMotionFrame(){retire();}
void ScreenMotionFrame::retire()noexcept{scheduler.remove(callback);active=false;}
i32 ScreenMotionFrame::update(){const auto input=context();const i32 result=shake?shake->update(input):nudge->update(input);if(result==1&&(!shake||(input.game_available&&!(input.game_flags&0x77)))){offsets(shake?shake->world_offset:nudge->world_offset,shake?shake->screen_offset:nudge->screen_offset);}return result;}
}
