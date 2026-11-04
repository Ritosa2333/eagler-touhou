#include "AsciiDrawSchedule.hpp"
namespace th17 {
AsciiDrawSchedule::AsciiDrawSchedule(FrameScheduler& f,AsciiText& t,AnmRenderer& r,ScreenViews& v):scheduler(f),text(t),renderer(r),views(v){
    static constexpr i32 priorities[]={5,54,67,82};for(u32 i=0;i<callbacks.size();i++){auto& c=callbacks[i];c.owner=this;c.index=i;c.frame.owner=&c;c.frame.enabled=true;c.frame.run=[](void* p)->i32{auto& c=*static_cast<Callback*>(p);return c.owner->pass(c.index)?1:5;};if(scheduler.add(c.frame,i?FramePass::Draw:FramePass::Update,priorities[i])<0)error="Text callback registration failed";}
}
AsciiDrawSchedule::~AsciiDrawSchedule(){for(auto& c:callbacks)scheduler.remove(c.frame);}
bool AsciiDrawSchedule::pass(u32 index){
    if(!error.empty())return false;if(!index){text.update();return true;}
    if(index<3){renderer.flush();if(!views.camera(DrawCamera::Hud,false)){error="Text camera selection failed";return false;}text.scene_origin(index==1);}
    const bool ok=text.draw(index==3?0:i32(index),renderer);if(index<3){text.scene_origin(false);renderer.flush();views.camera(DrawCamera::Fullscreen,false);}if(!ok)error=text.error;return ok;
}
}
