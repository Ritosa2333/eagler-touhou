#include "AsciiFrame.hpp"
namespace th13 {
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
 // 00404b50 submits queue1 without adding a game origin or selecting the
 // final HUD viewport. At priority51 the intermediate @R is still bound;
 // 43c7e0 switches to the backbuffer at52 and43ca50/text.anm84 composes at53.
 // Keep absolute RT coordinates (00403da0), including History x452. The
 // final [128,512] -> [32,416] crop supplies the -96 translation once.
 if(space==1){
  renderer.flush();
  if(!views.camera(DrawCamera::Fullscreen,false)){error="Caption render-target camera failed";return false;}
  renderer.offset={};
 }else if(space!=0&&!views.camera(DrawCamera::Hud,false)){error="Caption playfield camera failed";return false;}
 const bool rendered=text.draw(space,renderer);
 if(!rendered){error=text.error;return false;}
 if(!views.camera(DrawCamera::Fullscreen,false)){error="Caption full-screen camera failed";return false;}
 if(space==1)renderer.flush();
 // The playfield callbacks also restore the viewport after drawing the queue.
 if(space!=0&&!views.camera(DrawCamera::Fullscreen,false)){error="Caption viewport restore failed";return false;}
 return true;
}
}
