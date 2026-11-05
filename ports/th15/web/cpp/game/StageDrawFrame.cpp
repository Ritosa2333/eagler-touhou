#include "StageDrawFrame.hpp"
namespace th15 {
using namespace touhou::graphics;
StageDrawFrame::StageDrawFrame(FrameScheduler& f,StageScene& s,AnmManager& a,AnmRenderer& r,ZunGraphics& g,ScreenViews& v,StageDrawServices& h):scheduler(f),scene(s),animations(a),renderer(r),graphics(g),views(v),services(h),viewport(v.view(DrawCamera::Playfield).viewport){
 for(u32 i=0;i<callbacks.size();i++){auto& c=callbacks[i];c.owner=this;c.enabled=true;c.run=i?[](void* p)->i32{return static_cast<StageDrawFrame*>(p)->draw(1)?1:5;}:[](void* p)->i32{return static_cast<StageDrawFrame*>(p)->draw(0)?1:5;};if(scheduler.add(c,FramePass::Draw,i?6:3)<0)fail("Background draw registration failed");}
}
StageDrawFrame::~StageDrawFrame(){for(auto& c:callbacks)scheduler.remove(c);}
bool StageDrawFrame::fail(const std::string& value){if(error.empty())error=value;return false;}
void StageDrawFrame::prepare_camera(){
 renderer.flush();auto camera=scene.script.state.camera;camera.direction=scene.camera_direction();auto& v=views.view(DrawCamera::Playfield);v.viewport=viewport;v.camera=stage_camera(camera,viewport);renderer.set_viewport(v.viewport);renderer.set_camera(v.camera);graphics.presentation_camera(true);renderer.offset=v.offset;
}
void StageDrawFrame::fog_parameters(){const auto& fog=scene.script.state.camera.fog;graphics.set_fog_color(fog.color);graphics.set_fog_range(fog.near_distance,fog.far_distance);}
bool StageDrawFrame::layers(i32 first,i32 last){for(i32 layer=first;layer<=last;layer++)if(!scene.draw(renderer,graphics,layer,viewport,playfield_origin))return fail(scene.error);renderer.flush();return true;}
bool StageDrawFrame::background(){
 if(!(state.flags&4)||state.transition.current<60){prepare_camera();graphics.set_depth_mask(true);renderer.flush();graphics.set_depth_compare(Compare::LessEqual);fog_parameters();const u32 color=(state.flags&4)&&signed_bits(scene.frames)<34?0:scene.script.state.camera.fog.color;if(!graphics.clear_target(color,&viewport))return fail("Background region clear failed");}
 if(state.flags&4){if(state.transition.current<30){if(!services.background_fade(30,20,10))return fail("Background fade creation failed");state.flags|=1;state.transition.set(1);}else{state.flags&=~1u;state.tint_color&=0x00ffffff;}}
 if(state.tint_color>>24){renderer.tint_enabled=true;renderer.tint=state.tint_color;state.tint_color&=0x00ffffff;}
 scene.drawn_instances=scene.culled_instances=scene.drawn_primitives=0;scene.draw_objects=(state.flags&1)!=0;
 if(state.flags&1){graphics.set_fog(true);if(!layers(0,7))return false;}
 renderer.tint_enabled=false;renderer.tint=0x80808080;renderer.flush();graphics.set_depth_mask(false);renderer.flush();graphics.set_depth_compare(Compare::Always);return true;
}
bool StageDrawFrame::foreground(){
 if(!(state.flags&4)||state.transition.current<60){prepare_camera();graphics.set_fog(false);graphics.set_depth_mask(false);graphics.set_depth_compare(Compare::Always);if(!renderer.draw_layer(animations.registry.layer(31)))return fail(renderer.error);renderer.flush();graphics.set_depth_compare(Compare::LessEqual);if(!renderer.draw_layer(animations.registry.layer(32)))return fail(renderer.error);renderer.flush();graphics.set_depth_compare(Compare::LessEqual);fog_parameters();}
 if((state.flags&4)&&state.transition.current>=30)state.tint_color&=0x00ffffff;
 scene.draw_objects=(state.flags&1)!=0;if(state.flags&1){renderer.flush();graphics.set_depth_mask(false);graphics.set_fog(true);if(!layers(8,11))return false;}
 renderer.tint_enabled=false;renderer.tint=0x80808080;
 if(state.transition.current>0){state.transition.decrement(&rate);if(state.transition.current<=0){state.tint_color|=0xff000000;if(state.flags&2)state.flags|=8;state.flags&=~6u;state.tint_color=0x00ffffff;}}
 renderer.flush();graphics.set_depth_mask(false);renderer.flush();graphics.set_depth_compare(Compare::Always);renderer.flush();graphics.set_fog(false);return true;
}
bool StageDrawFrame::draw(u32 pass){if(!error.empty())return false;if(pass>1)return fail("Background draw pass outside schedule");if(!scene.ready())return true;state.flags=(state.flags&~1u)|(scene.draw_objects?1u:0u);if(state.flags&8)return true;return pass?foreground():background();}
}
