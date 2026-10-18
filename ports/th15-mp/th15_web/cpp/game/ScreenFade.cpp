#include "ScreenFade.hpp"
#include <algorithm>
namespace th15 {
using namespace touhou::graphics;
ScreenFade::ScreenFade(FrameScheduler& f,AnmRenderer& r,ZunGraphics& g,AnmEnvironment& e,i32 duration,i32 up,i32 dp,u32 color,bool cover,bool full):scheduler(f),renderer(r),graphics(g),environment(e),covering(cover),full_screen(full){
 state.duration=duration;state.color=color;state.alpha=cover||full?0:255;state.age.set(0);auto& u=callbacks[0];u.owner=this;u.enabled=true;u.run=[](void* p)->i32{return static_cast<ScreenFade*>(p)->update();};u.cleanup=[](void* p)->i32{static_cast<ScreenFade*>(p)->retire();return 0;};scheduler.add(u,FramePass::Update,up);
 auto& d=callbacks[1];d.owner=this;d.enabled=true;d.run=[](void* p)->i32{return static_cast<ScreenFade*>(p)->draw()?1:5;};scheduler.add(d,FramePass::Draw,dp);
}
ScreenFade::~ScreenFade(){retire();}
void ScreenFade::retire()noexcept{for(auto& c:callbacks)scheduler.remove(c);active=false;}
i32 ScreenFade::update(){
 if(context){const auto input=context();shutdown=input.shutdown;game_available=input.game_available;game_flags=input.game_flags;rate=input.rate;}
 if(shutdown)return i32(FrameAction::Cleanup);
 if(covering){if(state.duration){state.alpha=state.age.current<state.duration?truncate_int(float(float(state.age.fractional*255.f)/float(state.duration))):255;state.alpha=std::max(0,state.alpha);}if(state.age.current>=wrapping_add(state.duration,2))return i32(FrameAction::Cleanup);if(!game_available||!(game_flags&5))state.age.tick(&rate);}
 else{if(state.duration){const float fraction=float(float(state.age.fractional*255.f)/float(state.duration));state.alpha=std::max(0,truncate_int(float(255.f-fraction)));}if(state.age.current>=state.duration)return i32(FrameAction::Cleanup);state.age.tick(&rate);}
 return i32(FrameAction::Continue);
}
bool ScreenFade::draw(){
 if(!active)return true;renderer.flush();if(full_screen)graphics.set_viewport({0,0,environment.screen_width,environment.screen_height,0,1});auto& s=graphics.pipeline();s.color.operation=s.alpha.operation=ColorOperation::First;s.color.first=s.alpha.first={ArgumentSource::Diffuse};s.color.second=s.alpha.second={ArgumentSource::Diffuse};s.textureTransform=false;s.destinationBlend=BlendFactor::InverseSourceAlpha;
 const u32 color=(u32(state.alpha)<<24)|state.color;const float w=float(environment.screen_width),h=float(environment.screen_height);struct Vertex{Vec3 position;float reciprocal_w;u32 color;};const Vertex vertices[]={{{0,0,0},1,color},{{w,0,0},1,color},{{0,h,0},1,color},{{w,h,0},1,color}};presentation_vm.visual.flags=3;presentation_vm.interpolators.alpha.duration=state.duration;presentation_vm.age=state.age;graphics.presentation_range(presentation_vm,0,4);graphics.set_layout(VertexLayout::ScreenColor);graphics.primitives(Topology::Strip,2,vertices,sizeof(Vertex));
 s.color.operation=s.alpha.operation=ColorOperation::Multiply;s.color.first=s.alpha.first={ArgumentSource::Texture};s.color.second=s.alpha.second={ArgumentSource::Diffuse};renderer.invalidate();return true;
}
}
