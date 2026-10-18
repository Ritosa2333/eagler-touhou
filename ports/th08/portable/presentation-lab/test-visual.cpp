#include "../../th08_web/cpp/game/PresentationVisual.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace th08;
using V=presentation::VisualSample;
static AnmVm sample(){AnmVm v;v.visible=1;v.anmFile=reinterpret_cast<AnmLoaded*>(1);v.beginningOfScript=reinterpret_cast<AnmRawInstr*>(2);v.scriptIndex=3;v.scale={1,1};v.color1.d3dColor=-1;return v;}
static bool near(float a,float b){return std::fabs(a-b)<.00001f;}
int main(){
    unsigned tests=0;
    auto previous=sample(),current=previous;
    previous.interpEndTimers[AnmInterp_Scale].current=20;current=previous;current.scale={2,3};
    const auto original=current;auto draw=current;V before(previous);
    presentation::begin(.5f,true,true);before.apply(current,draw,presentation::alpha);
    assert(near(draw.scale.x,1.5f)&&near(draw.scale.y,2)&&draw.updateScale);++tests;
    assert(std::memcmp(&current,&original,sizeof(current))==0);++tests;
    current.interpEndTimers[AnmInterp_Scale].current=0;draw=current;before.apply(current,draw,.5f);assert(near(draw.scale.x,1.5f));++tests;
    draw=current;before.apply(current,draw,1.f);assert(std::memcmp(&draw,&current,sizeof(draw))==0);++tests;
    draw=current;presentation::begin(.5f,true,false);before.apply(current,draw,.5f);assert(std::memcmp(&draw,&current,sizeof(draw))==0);++tests;
    draw=current;presentation::begin(.5f,false,true);before.apply(current,draw,.5f);assert(near(draw.scale.x,2));++tests;
    presentation::begin(.5f,true,true);current.scale.x=-2;draw=current;before.apply(current,draw,.5f);assert(draw.scale.x==-2);++tests;
    current=sample();current.scale={2,2};previous=sample();draw=current;V(previous).apply(current,draw,.5f);assert(near(draw.scale.x,2));++tests;
    draw=current;V(previous).apply(current,draw,.5f,V::Attributes,V::Scale);assert(near(draw.scale.x,1.5f));++tests;
    current=sample();previous=current;previous.angleVel.z=.1f;current=previous;previous.rotation.z=3.1f;current.rotation.z=-3.1f;draw=current;V(previous).apply(current,draw,.5f);assert(near(draw.rotation.z,3.14159274f));++tests;
    previous=sample();previous.uvScrollVel.x=.1f;previous.uvScrollPos.x=.95f;current=previous;current.uvScrollPos.x=.05f;draw=current;V(previous).apply(current,draw,.5f);assert(near(draw.uvScrollPos.x,1.f));++tests;
    previous=sample();previous.interpEndTimers[AnmInterp_Alpha1].current=10;previous.color1.a=76;current=previous;current.color1.a=89;draw=current;V(previous).apply(current,draw,.5f);assert(draw.color1.a==83);++tests;
    current.beginningOfScript=reinterpret_cast<AnmRawInstr*>(3);draw=current;V(previous).apply(current,draw,.5f);assert(draw.color1.a==89);++tests;
    current=previous;current.color1.a=89;current.activeSpriteIndex=5;draw=current;V(previous).apply(current,draw,.5f);assert(draw.color1.a==83&&draw.activeSpriteIndex==5);++tests;
    current.visible=0;draw=current;V(previous).apply(current,draw,.5f);assert(draw.color1.a==89);++tests;
    previous=sample();previous.pos={10,20,0};current=previous;current.pos.x=14;draw=current;V(previous).apply(current,draw,.5f);assert(draw.pos.x==14);++tests;
    draw=current;V(previous).apply(current,draw,.5f,V::Position,V::Position);assert(draw.pos.x==12);++tests;
    current.pos.x=200;draw=current;V(previous).apply(current,draw,.5f,V::Position,V::Position);assert(draw.pos.x==200);++tests;
    presentation::begin(1.f,true,true);assert(presentation::lerp(1.e20f,1.f)==1.f&&presentation::lerp_world(1.e20f,1.f)==1.f);++tests;
    presentation::begin(0.f,true,true);assert(presentation::lerp(1.f,1.e20f)==1.f&&presentation::lerp_world(1.f,1.e20f)==1.f);++tests;
    presentation::SnapshotMarker marker;
    assert(marker.capture()&&marker.capture());++tests; // standalone owner tests
    {
        presentation::CalculationScope tick;
        assert(marker.capture()&&!marker.capture());++tests;
        {presentation::CalculationScope nested;assert(!marker.capture());++tests;}
        assert(!marker.capture());++tests;
    }
    {presentation::CalculationScope next_tick;assert(marker.capture());++tests;}
    presentation::begin(.5f,true,true);previous=sample();current=previous;current.rotation.z=.04f;draw=current;
    V(previous).apply(current,draw,.5f,V::Attributes,V::Rotation);assert(near(draw.rotation.z,.02f));++tests;
    previous=sample();previous.interpEndTimers[AnmInterp_Alpha1].current=10;previous.color1.a=60;previous.color2.a=20;
    current=previous;current.flag17=1;current.color1.a=80;current.color2.a=90;draw=current;V active_before(previous);
    active_before.apply_active_opacity(current,draw,.5f);assert(draw.color2.a==75&&draw.color1.a==80);++tests;
    previous=sample();previous.flag17=1;previous.interpEndTimers[AnmInterp_Alpha1].current=10;previous.color2.a=40;
    current=previous;current.flag17=0;current.color1.a=80;draw=current;V(previous).apply_active_opacity(current,draw,.5f);
    assert(draw.color1.a==60&&draw.color2.a==40);++tests;
    previous=sample();previous.interpEndTimers[AnmInterp_Alpha1].current=10;previous.color1.a=129;
    current=previous;current.color1.a=160;draw=current;V(previous).apply_active_opacity(current,draw,.5f,V::Opacity,128,255);
    assert(std::abs(i32(draw.color1.a)*255/128-192)<=1);++tests;
    presentation::end();
    std::printf("PresentationVisual: %u real WASM assertions passed\n",tests);
}
