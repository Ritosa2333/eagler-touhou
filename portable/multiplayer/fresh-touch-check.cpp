#include "../input/TouchController.hpp"
#include "../../th08_web/cpp/multiplayer/AnalogMovement.hpp"
#include <eagler/netplay/NetplayCore.hpp>
#include <cassert>
#include <cstdio>
#include <array>
using namespace th08;
using namespace th08::multiplayer;
using namespace Netplay;

struct Pilot:PlayerMovementActions {
    PlayerMovementState state{};TouchRemainder remainder{};AnalogInput input{};
    Vec2 minimum{-184,32},extent{368,400};FrameTiming timing{};
    Pilot(){state.position={0,200,0};}
    void pose(i32)override{}
    bool movement(const PlayerMovementState& s,float speed,const FrameTiming& t,float& x,float& y)override{
        return ResolveAnalogMovement(input,remainder,s,minimum,extent,speed,t,x,y);
    }
    void step(FrameInput sample){
        assert(ValidInputSample(sample));input=MovementSample(sample);
        auto before=state.position;ShotProfile normal{},focus{};normal.normal_speed=4;focus.focus_speed=2;
        move_player(state,normal,focus,(sample.buttons&4)!=0,0,sample.buttons,minimum,extent,timing,this);
        if(remainder.active){
            remainder.x=Scalar::sub(remainder.x,Scalar::sub(state.position.x,before.x));
            remainder.y=Scalar::sub(remainder.y,Scalar::sub(state.position.y,before.y));
        }
    }
};
FrameInput delta(float x,float y=0,bool begin=false){FrameInput f;f.analogMode=begin?AnalogMode::DirectTouchBegin:AnalogMode::DirectTouchDelta;f.x=x;f.y=y;f.touchUsed=true;return f;}
void producer(){
    touhou::input::TouchState s;s.context=1;s.instance=1;s.ready=true;s.x=0;s.y=200;s.fast=4;s.slow=2;s.min_x=-184;s.max_x=184;s.min_y=32;s.max_y=432;
    touhou::input::TouchController t;float x,y;bool begin;
    t.pointer(0,1,.5f,.5f,0,s,false);t.pointer(1,1,.5375f,.5f,1,s,false);
    t.sample(s,2,false,false);assert(t.take_player_delta(s,x,y,begin)&&begin&&std::abs(x-24)<.0001f&&y==0);
    // Correcting the simulation position must not manufacture a device delta.
    s.x=-20;t.sample(s,3,false,false);assert(t.take_player_delta(s,x,y,begin)&&!begin&&x==0&&y==0);
    t.pointer(1,1,.55f,.5f,4,s,true);t.sample(s,5,true,false);
    assert(t.take_player_delta(s,x,y,begin)&&!begin&&std::abs(x-4)<.0001f);
    t.pointer(1,1,1.f,.5f,6,s,false);t.sample(s,7,false,false);
    assert(t.take_player_delta(s,x,y,begin)&&std::abs(x-156)<.0001f);
    t.pointer(1,1,1.1f,.5f,8,s,false);t.sample(s,9,false,false);assert(t.take_player_delta(s,x,y,begin)&&x==0);
    t.pointer(1,1,1.0875f,.5f,10,s,false);t.sample(s,11,false,false);assert(t.take_player_delta(s,x,y,begin)&&std::abs(x+8)<.0001f);
    s.ready=false;t.sample(s,12,false,false);assert(!t.take_player_delta(s,x,y,begin));
    s.ready=true;s.x=0;t.sample(s,13,false,false);assert(t.take_player_delta(s,x,y,begin)&&begin&&x==0);
    t.pointer(2,1,1.0875f,.5f,14,s,false);assert(!t.take_player_delta(s,x,y,begin));
    t.pointer(0,1,.5f,.5f,15,s,false);t.sample(s,16,false,false);assert(t.take_player_delta(s,x,y,begin)&&begin&&x==0);
    t.sample(s,17,false,true);assert(!t.take_player_delta(s,x,y,begin));
    t.pointer(0,2,.5f,.5f,18,s,false);s.context=2;t.sample(s,19,false,false);assert(!t.take_player_delta(s,x,y,begin));
    s.context=1;t.pointer(0,3,.5f,.5f,20,s,false);t.pointer(1,3,.55f,.5f,21,s,false);
    t.cancel_transient();assert(!t.take_player_delta(s,x,y,begin));
    t.pointer(0,4,.5f,.5f,22,s,false);t.set_mode(3);assert(!t.take_player_delta(s,x,y,begin));
    t.set_mode(0);t.pointer(0,5,.5f,.5f,23,s,false);t.pointer(1,5,.55f,.5f,24,s,false);
    // A stage/player instance change must not carry the old unsampled drag.
    ++s.instance;s.x=40;t.pointer(1,5,.6f,.5f,25,s,false);t.sample(s,26,false,false);
    assert(t.take_player_delta(s,x,y,begin)&&begin&&x==0&&y==0);
    t.pointer(1,5,.6125f,.5f,27,s,false);t.sample(s,28,false,false);
    assert(t.take_player_delta(s,x,y,begin)&&!begin&&std::abs(x-8)<.0001f);
    t.begin_session();assert(!t.take_player_delta(s,x,y,begin));
}
void movement(){
    Pilot p;p.step(delta(24,0,true));assert(p.state.position.x==4&&p.remainder.x==20);
    for(int i=0;i<5;++i)p.step(delta(0));assert(p.state.position.x==24&&p.remainder.x==0);
    p.step(delta(24));p.step(delta(-8,0,true));assert(p.state.position.x==24&&p.remainder.x==-4);
    p.step({});assert(!p.remainder.active&&p.state.position.x==24);
    p.state.position.x=180;p.step(delta(1000,0,true));assert(p.state.position.x==184&&p.remainder.x==0);
    p.step(delta(-8));assert(p.state.position.x==180&&p.remainder.x==-4);
    auto unlimited=delta(100,0,true);unlimited.unlimited=true;p.step(unlimited);assert(p.state.position.x==184&&p.remainder.x==0);
    p.state.position.x=0;p.state.multiplier.x=.5f;p.timing.rate=.5f;p.step(delta(8,0,true));assert(p.state.position.x==1&&p.remainder.x==7);
    auto focused=delta(0);focused.buttons=4;p.step(focused);assert(p.state.position.x==1.5f&&p.remainder.x==6.5f);
    // Old Replay samples retain the exact legacy movement operation.
    FrameInput legacy;legacy.analogMode=AnalogMode::DirectTouch;legacy.x=4;p.step(legacy);assert(!p.remainder.active&&p.state.position.x==2.5f);
}
struct Counts {int corrections=0,replayed=0;};
Counts delayed(const std::array<FrameInput,180>& samples){
    RollbackCore core;CoreConfig config;config.sessionId=7;config.playerCount=2;config.localPlayer=0;config.maxRollbackFrames=12;
    assert(core.Reset(config));Pilot predicted,oracle;std::array<Pilot,181> saved;
    std::array<Pilot,180> expected;for(unsigned f=0;f<samples.size();++f){oracle.step(samples[f]);expected[f]=oracle;}
    unsigned next=0;Counts count;
    auto step=[&](unsigned f){saved[f]=predicted;auto decision=core.PrepareFrame(f);assert(decision.canAdvance);predicted.step(decision.inputs[1]);saved[f+1]=predicted;assert(core.MarkSimulated(f,decision));};
    auto correct=[&](){if(!core.HasRollbackRequest())return;auto first=core.RollbackFrame();assert(first<next);predicted=saved[first];assert(core.RewindSimulationTo(first));++count.corrections;
        for(auto f=first;f<next;++f){step(f);++count.replayed;}
    };
    for(unsigned wall=0;wall<samples.size()+3;++wall){
        if(wall>=3)core.SubmitRemoteInput(1,wall-3,samples[wall-3]);correct();
        if(wall<samples.size()){assert(core.ScheduleLocalInput(wall,FrameInput{}));step(wall);++next;}
    }
    assert(core.ConfirmedThrough(1)==samples.size()-1);
    for(unsigned f=0;f<samples.size();++f){
        assert(saved[f+1].state.position.x==expected[f].state.position.x&&saved[f+1].state.position.y==expected[f].state.position.y);
        assert(saved[f+1].remainder.x==expected[f].remainder.x&&saved[f+1].remainder.y==expected[f].remainder.y&&saved[f+1].remainder.active==expected[f].remainder.active);
    }
    assert(predicted.state.position.x==oracle.state.position.x&&predicted.state.position.y==oracle.state.position.y);
    assert(predicted.remainder.x==oracle.remainder.x&&predicted.remainder.y==oracle.remainder.y&&predicted.remainder.active==oracle.remainder.active);
    return count;
}
int main(){
    producer();movement();std::array<FrameInput,180> legacy{},fresh{};Pilot old,updated;float target=0;
    for(unsigned f=0;f<fresh.size();++f){
        float change=f%30==5?24:f%30==20?-24:0;target+=change;
        fresh[f]=delta(change,0,f==0);legacy[f]=fresh[f];legacy[f].analogMode=AnalogMode::DirectTouch;legacy[f].x=target-old.state.position.x;
        old.step(legacy[f]);updated.step(fresh[f]);assert(old.state.position.x==updated.state.position.x&&old.state.position.y==updated.state.position.y);
    }
    const auto a=delayed(legacy),b=delayed(fresh);assert(b.corrections<a.corrections&&b.replayed<a.replayed);
    std::printf("{\"passed\":true,\"frames\":180,\"delayFrames\":3,\"legacyCorrections\":%d,\"freshCorrections\":%d,\"legacyResimulated\":%d,\"freshResimulated\":%d}\n",a.corrections,b.corrections,a.replayed,b.replayed);
}
