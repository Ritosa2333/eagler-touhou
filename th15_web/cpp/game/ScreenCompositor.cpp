#include "ScreenCompositor.hpp"
namespace th15 {
ScreenCompositor::ScreenCompositor(FrameScheduler& f,AnmManager& a,AnmEnvironment& e,AnmRenderer& r,ZunGraphics& g,ScreenViews& v):scheduler(f),animations(a),environment(e),renderer(r),graphics(g),views(v){
 for(u32 i=0;i<callbacks.size();i++){auto& c=callbacks[i];c.owner=this;c.pass=i;c.frame.owner=&c;c.frame.enabled=true;c.frame.run=[](void* p){auto& c=*static_cast<Callback*>(p);return c.owner->run(c.pass)?1:5;};if(scheduler.add(c.frame,FramePass::Draw,priorities[i])<0)fail("Screen compositor callback registration failed");}
}
ScreenCompositor::~ScreenCompositor(){for(auto& c:callbacks)scheduler.remove(c.frame);}
bool ScreenCompositor::fail(const char* value){if(error.empty())error=value;return false;}
bool ScreenCompositor::select(const ScreenSurface& target){renderer.flush();return graphics.select_target(target.resource,target.texture)||fail("Screen compositor target selection failed");}
bool ScreenCompositor::compose(u32 index,bool camera,bool layer){
 if(!captures[index])return fail("Screen compositor capture animation missing");
 renderer.flush();graphics.pipeline().blendEquation=touhou::graphics::BlendEquation::Add;graphics.pipeline().alphaTest=false;graphics.set_depth_mask(false);
 if(camera&&!views.camera(DrawCamera::Playfield,false))return fail("Screen compositor camera selection failed");
 // The native capture scripts use ONE/ZERO + ADD (mode 3): destination
 // pixels, including alpha, must not contribute. Submit this copy without
 // blending, then restore the scene state before layer 33. This is equivalent
 // to the authored copy, without relying on mobile blend-state transitions.
 const bool source_copy=captures[index]->visual.blend_mode()==3&&!graphics.pipeline().separateAlphaBlend;
 const bool blending=graphics.pipeline().blend;
 if(source_copy)graphics.pipeline().blend=false;
 const int result=renderer.draw(*captures[index]);
 if(source_copy){renderer.flush();graphics.pipeline().blend=blending;}
 if(result==-2)return fail(renderer.error.c_str());captures[index]->visual.color=0xffffffff;
 if(layer&&!renderer.draw_layer(animations.registry.layer(33)))return fail(renderer.error.c_str());
 renderer.flush();graphics.pipeline().alphaTest=true;return true;
}
bool ScreenCompositor::run(u32 pass){
 if(!error.empty())return false;
 const auto& region=views.view(DrawCamera::Playfield).viewport;
 switch(pass){
 case 0:
  if(active){// Shot culling uses the 384x448 content, not the 408x472 border camera.
   environment.playfield_origin={float(environment.screen_offsets[0])-192.f,float(environment.screen_offsets[1])};if(!graphics.clear_target(0xffffffff,nullptr)||!select(main)||!graphics.clear_target(clear_color,&region))return fail("Screen compositor frame clear failed");playfield_size={384,448};environment.screen_offsets[2]=i32(environment.screen_width)/2;environment.screen_offsets[3]=truncate_int(float(float(environment.screen_height)-448.f))/2;}
  else if(!graphics.clear_target(clear_color,nullptr))return fail("Screen compositor frame clear failed");
  renderer.invalidate();renderer.tint_enabled=false;renderer.tint=0x80808080;renderer.offset={};return views.camera(DrawCamera::Fullscreen,false)||fail("Screen compositor full-screen camera failed");
 case 1:if(!active)return true;return select(alternate)&&(graphics.clear_depth(&region)||fail("Screen compositor depth clear failed"));
 case 2:if(!active)return true;if(replacement[0])return replacement[0]()||fail("Screen compositor background replacement failed");return compose(0,true,true);
 case 3:if(!active)return true;return select(main)&&(graphics.clear_depth(&region)||fail("Screen compositor depth clear failed"));
 case 4:if(!active)return true;if(replacement[1])return replacement[1]()||fail("Screen compositor playfield replacement failed");return compose(1,true,false);
 case 5:if(!active)return true;if(!select(alternate)||!graphics.clear_depth(&views.view(DrawCamera::Interface).viewport))return fail("Screen compositor HUD depth clear failed");playfield_size={float(truncate_int(float(384.f*environment.resolution_scale))),float(truncate_int(float(448.f*environment.resolution_scale)))};return true;
 case 6:if(!active)return true;return compose(2,false,false);
 case 7:return select({nullptr,0})&&(graphics.clear_target(0xff000000,nullptr)||fail("Screen compositor presentation clear failed"));
 case 8:if(!active)return true;if(!captures[3])return fail("Screen compositor presentation animation missing");renderer.flush();graphics.pipeline().alphaTest=false;if(renderer.draw(*captures[3])==-2)return fail(renderer.error.c_str());captures[3]->visual.color=0xffffffff;renderer.flush();graphics.pipeline().alphaTest=true;return true;
 case 9:renderer.flush();views.view(DrawCamera::Playfield).offset={};views.view(DrawCamera::Interface).offset={};return true;
 default:return fail("Screen compositor pass outside schedule");
 }
}
}
