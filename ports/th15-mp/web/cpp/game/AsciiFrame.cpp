#include "AsciiFrame.hpp"
namespace th15 {
AsciiFrame::AsciiFrame(FrameScheduler& s,AsciiText& t,AnmRenderer& r,AnmDrawServices& v):scheduler(s),text(t),renderer(r),views(v){
 constexpr i32 priorities[]{4,78,51,63};
 for(u32 i=0;i<callbacks.size();i++){auto& c=callbacks[i];c.owner=this;c.enabled=true;
  if(i==0)c.run=[](void* p){auto& f=*static_cast<AsciiFrame*>(p);f.text.update();f.frames=wrapping_add(f.frames,1);return i32(FrameAction::Continue);};
  else if(i==1)c.run=[](void* p){return static_cast<AsciiFrame*>(p)->draw(0)?1:5;};
  else if(i==2)c.run=[](void* p){return static_cast<AsciiFrame*>(p)->draw(1)?1:5;};
  else c.run=[](void* p){return static_cast<AsciiFrame*>(p)->draw(2)?1:5;};
  scheduler.add(c,i==0?FramePass::Update:FramePass::Draw,priorities[i]);
 }
}
AsciiFrame::~AsciiFrame(){for(auto& c:callbacks)scheduler.remove(c);}
bool AsciiFrame::draw(i32 space){
 if(space!=0&&!views.camera(DrawCamera::Hud,false)){error="Caption playfield camera failed";return false;}
 if(space==1){renderer.flush();text.scene_origin(true);}
 const bool rendered=text.draw(space,renderer);
 if(space==1)text.scene_origin(false);
 if(!rendered){error=text.error;return false;}
 if(!views.camera(DrawCamera::Fullscreen,false)){error="Caption full-screen camera failed";return false;}
 if(space==1)renderer.flush();
 // The playfield callbacks also restore the viewport after drawing the queue.
 if(space!=0&&!views.camera(DrawCamera::Fullscreen,false)){error="Caption viewport restore failed";return false;}
 return true;
}
}
