#include "AnmSceneEffects.hpp"
#include "AnmOverlay.hpp"
#include "VectorMath.hpp"
#include <cmath>
namespace th15 {
namespace {
Vec3 plus(Vec3 a,Vec3 b){return {float(a.x+b.x),float(a.y+b.y),float(a.z+b.z)};}
Vec3 minus(Vec3 a,Vec3 b){return {float(a.x-b.x),float(a.y-b.y),float(a.z-b.z)};}
Vec3 times(Vec3 a,float b){return {float(a.x*b),float(a.y*b),float(a.z*b)};}
Vec3 normalize3(Vec3 a){const float sq=float(float(float(a.x*a.x)+float(a.y*a.y))+float(a.z*a.z));if(sq<0x1p-46f)return {};const float inv=float(1.f/float(std::sqrt(double(sq)))),fix=float(3.f-float(float(sq*inv)*inv)),scale=float(float(.5f*inv)*fix);return times(a,scale);}
Vec3 radial(float angle,float radius){return {float(std::cos(double(angle))*double(radius)),float(std::sin(double(angle))*double(radius)),0};}
void curve(AnmVm& vm,i32 frames,Vec3 a,Vec3 da,Vec3 b,Vec3 db){auto& p=vm.interpolators.position;p.begin(frames,8,{a.x,a.y,a.z},{b.x,b.y,b.z});p.control1={da.x,da.y,da.z};p.control2={db.x,db.y,db.z};}
}
AnmSceneEffects::AnmSceneEffects(AnmManager& m,Rng& g,Rng& v,i32 bank):animations(m),game(g),visual(v),effect_bank(bank),previous_create(std::move(m.effect)),previous_update(std::move(m.effect_update)),previous_interrupt(std::move(m.effect_interrupt)){
 animations.effect=[this](AnmVm& vm,i32 kind){return configure(vm,kind);};animations.effect_update=[this](AnmVm& vm,float rate){return update(vm,rate);};animations.effect_interrupt=[this](AnmVm& vm,i32 label,float rate){return interrupt(vm,label,rate);};
}
AnmSceneEffects::~AnmSceneEffects(){animations.effect=std::move(previous_create);animations.effect_update=std::move(previous_update);animations.effect_interrupt=std::move(previous_interrupt);}
bool AnmSceneEffects::configure(AnmVm& vm,i32 kind){
 if(kind==0){vm.geometry.clear();vm.geometry.overlay=std::make_unique<AnmOverlay>();vm.geometry.allocation_bytes=0x1e30;auto& overlay=*vm.geometry.overlay;
  vm.visual.render_flags&=~0xc0000u;vm.visual.layer=0;for(u32 i=0;i<5;i++){auto panel=std::make_unique<AnmVm>();if(!animations.bind_template(*panel,effect_bank,i<4?i32(i)+3:196)||animations.tick_instance(*panel)<0)return false;if(i<4)panel->visual.translation={320,240,0};overlay.panels[i]=std::move(panel);}
  vm.visual.layer=39;vm.visual.render_flags=(vm.visual.render_flags&~0x6c0000u)|0x100000;return true;}
 if(kind==1){vm.geometry.clear();vm.geometry.gather=std::make_unique<AnmGatherEffect>();vm.geometry.gather->age.set(0);vm.geometry.allocation_bytes=0x193c;return true;}
 if(kind==3){vm.geometry.clear();vm.geometry.trail=std::make_unique<AnmTrail>();vm.geometry.trail->initialize(visual);vm.geometry.allocation_bytes=0x318;vm.visual.flags&=~0x1e0u;vm.visual.layer=19;vm.visual.render_flags=(vm.visual.render_flags&~0x80000u)|0x40000;return true;}
 if(kind==2){vm.geometry.clear();vm.geometry.trail=std::make_unique<AnmTrail>();vm.geometry.trail->initialize_orange(vm.visual.translation,visual);vm.geometry.allocation_bytes=0x318;vm.visual.flags=(vm.visual.flags&~0x1c0u)|0x20;vm.visual.layer=15;vm.visual.render_flags=(vm.visual.render_flags&~0x80000u)|0x40000;auto& alpha=vm.interpolators.alpha;alpha.control1={};alpha.control2={};alpha.begin(64,0,{255},{0});return true;}
 animations.error="Unimplemented animation effect descriptor "+std::to_string(kind);return false;
}
bool AnmSceneEffects::interrupt(AnmVm& vm,i32 label,float rate){
 if(vm.geometry.overlay){auto& overlay=*vm.geometry.overlay;if(label==1){for(u32 i=0;i<4;i++){if(!animations.bind_template(*overlay.panels[i],effect_bank,i32(i)+7)||animations.tick_instance(*overlay.panels[i])<0)return false;}}
  else if(label>=7&&label<=10){overlay.mode=label==8?1:label==10?3:0;vm.visual.layer=label==8?23:label==9?35:29;vm.visual.render_flags=(vm.visual.render_flags&~(label==8?0x640000u:0x6c0000u))|(label==8?0x180000:0x100000);for(u32 i=0;i<4;i++)overlay.panels[i]->visual.translation={320,240,0};}return true;}
 if(vm.geometry.gather&&label==1){auto& age=vm.geometry.gather->age;age.rate_index=0;age.previous=age.current;age.fractional=float(age.fractional+float((rate>.99f&&rate<1.01f?1.f:rate)*300.f));age.current=truncate_int(age.fractional);}return true;}
i32 AnmSceneEffects::update(AnmVm& vm,float rate){
 if(vm.geometry.overlay){auto& overlay=*vm.geometry.overlay;u32 finished=0;for(u32 i=0;i<4;i++){const i32 result=overlay.panels[i]->tick(visual,rate);if(result<0){vm.error=overlay.panels[i]->error;return -1;}if(result)finished++;}if(finished==4)return 1;const i32 result=overlay.panels[4]->tick(visual,rate);if(result<0){vm.error=overlay.panels[4]->error;return -1;}overlay.frames=wrapping_add(overlay.frames,1);return 0;}
 if(!vm.geometry.gather){vm.error="Animation gathering state unavailable";return -1;}auto& e=*vm.geometry.gather;
 e.centers.fill(vm.visual.translation);e.centers[1]=plus(e.centers[1],radial(vm.variables.rotation.z,300));e.centers[0]=plus(e.centers[0],radial(float(vm.variables.vector[2]+vm.variables.rotation.z),150));
 if(e.age.current!=e.age.previous&&e.age.current<50){
  if(e.age.current<0){vm.error="Negative gathering particle frame";return -1;}const u32 start=u32(e.age.current)*4;
  for(u32 j=0;j<4;j++){const u32 handle=animations.create(effect_bank,j==3?153:152,-1,0);auto* child=animations.registry.find(handle);if(!child){vm.error=animations.error;return -1;}e.handles[start+j]=handle;child->visual.color=vm.visual.color;child->variables.integers[0]=vm.variables.integers[0];if(j==3){const u32 value=vm.visual.color;child->visual.color=(value&0xff000000)|(u32(u8(44-u8(value>>16)))<<16)|(u32(u8(44-u8(value>>8)))<<8)|u8(44-u8(value));}}
 }
 u32 active=0;const auto random_radial=[&](float radius){const float angle=float(game.signed_unit()*3.1415927410125732f),distance=float(game.unit()*radius);return radial(angle,distance);};
 for(u32 i=0;i<e.handles.size();i++){
  auto* child=animations.registry.find(e.handles[i]);if(!child){e.handles[i]=0;continue;}child->slowdown=vm.effective_slowdown();
  if(!e.phase[i]){
   const Vec3 a=plus(e.centers[1],random_radial(150)),b=plus(e.centers[0],random_radial(50));
   const Vec3 direction=normalize3(minus(b,a)),end_direction=normalize3(minus(e.centers[2],b));
   const float end_length=float(float(game.unit()*200.f)+200.f);const Vec3 end_tangent=times(normalize3(plus(direction,end_direction)),end_length);
   const float start_length=float(float(game.unit()*100.f)+100.f);const Vec3 start_tangent=times(normalize3(minus(b,a)),start_length);
   curve(*child,vm.variables.integers[0],a,start_tangent,b,end_tangent);e.points[i]=b;e.tangents[i]=end_tangent;e.phase[i]=1;
  }else if(e.phase[i]==1&&child->age.current>=wrapping_add(vm.variables.integers[0],1)){
   const Vec3 end=plus(vm.visual.translation,random_radial(20)),tangent=random_radial(20);curve(*child,vm.variables.integers[0],e.points[i],e.tangents[i],end,tangent);e.phase[i]=2;
  }active++;
 }
 if(!active)return 1;e.age.tick(&rate);return 0;
}
}
