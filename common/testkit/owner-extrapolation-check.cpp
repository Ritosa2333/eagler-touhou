#include <eagler/presentation/Extrapolation.hpp>
#include <cassert>
#include <cmath>
#include <cstring>
#include <cstdio>
#if defined(TH08_OWNER_CHECK)
#include "PresentationVisual.hpp"
using namespace th08;
int main(){
 AnmVm current{};current.anmFile=reinterpret_cast<AnmLoaded*>(0x1000);current.visible=true;current.scriptIndex=1;current.activeSpriteIndex=2;
 current.pos={10,20,0};current.scale={1,1};current.color1.a=245;current.interpEndTimers[AnmInterp_Alpha1].current=10;
 presentation::VisualSample previous(current);current.pos={14,24,0};current.color1.a=255;
 const AnmVm saved=current;AnmVm draw=current;
 presentation::begin(eagler::presentation::ExtrapolationWeight(.5),true,true);
 previous.apply(current,draw,presentation::world_alpha,presentation::VisualSample::Position|presentation::VisualSample::Opacity,presentation::VisualSample::Position);
 assert(draw.pos.x==16&&draw.pos.y==26&&draw.color1.a==255);
 assert(std::memcmp(&saved,&current,sizeof current)==0);
 AnmVm again=current;previous.apply(current,again,presentation::world_alpha,presentation::VisualSample::Position,presentation::VisualSample::Position);
 assert(again.pos.x==draw.pos.x);
 AnmVm changed=current;changed.scriptIndex=2;AnmVm changed_draw=changed;previous.apply(changed,changed_draw,presentation::world_alpha,presentation::VisualSample::Position,presentation::VisualSample::Position);assert(changed_draw.pos.x==14);
 changed=current;changed.pos.x=400;changed_draw=changed;previous.apply(changed,changed_draw,presentation::world_alpha,presentation::VisualSample::Position,presentation::VisualSample::Position);assert(changed_draw.pos.x==400);
 presentation::begin(1.5f,true,true,false);draw=current;previous.apply(current,draw,presentation::world_alpha,presentation::VisualSample::Position,presentation::VisualSample::Position);assert(draw.pos.x==14);
 assert(std::abs(presentation::VisualSample::cyclic(.98f,.02f,1.5f,1.f)-1.04f)<.001f);
 presentation::end();std::puts("TH08 owner extrapolation: PASS (motion, lifecycle, teleport, frozen world, UV wrap, opacity clamp, state purity)");
}
#else
#include "HighRefresh.hpp"
#if defined(TH10_OWNER_CHECK)
namespace presentation=th10::high_refresh;
#else
namespace presentation=th11::high_refresh;
#endif
int main(){
 presentation::begin(eagler::presentation::ExtrapolationWeight(.5),true,true);
 assert(presentation::lerp(10,14)==16);assert(presentation::lerp_world(10,14)==16);
 presentation::begin(1.5f,true,true,false);assert(presentation::lerp_world(10,14)==14);
 presentation::begin(1.5f,false,true);assert(presentation::lerp(10,14)==14);
 presentation::end();assert(presentation::alpha==1&&!presentation::active);
 std::puts("native owner weight: PASS (forward motion, frozen world, disabled smoothing)");
}
#endif
