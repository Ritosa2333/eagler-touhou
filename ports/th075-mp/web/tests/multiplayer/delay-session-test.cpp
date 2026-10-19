#include "../../cpp/multiplayer/DelaySession.hpp"
#include "../../cpp/multiplayer/RollbackAudio.hpp"
#include "../../cpp/multiplayer/InputLatch.hpp"
#include <cassert>
#include <cstdio>
#include <functional>
#include "../../../portable/sdl/FrameCadence.hpp"

namespace {
using namespace Netplay;
struct Wire:PeerTransport {
    struct Delivery {std::uint64_t due;std::vector<std::uint8_t> bytes;};
    Wire* peer=nullptr;std::uint64_t now=0,inputLag=0;unsigned sends=0;bool disconnected=false;
    std::deque<Delivery> input;
    bool IsOpen()const override{return !disconnected;}
    bool Failed()const override{return false;}
    bool Disconnected()const override{return disconnected;}
    bool send(const std::uint8_t* p,std::size_t n,bool reliable){
        ++sends;
        // Drop/reorder/duplicate the actual input lane, including calibration.
        if(!reliable&&sends%23==0)return true;
        const auto due=now+(reliable?4000:(sends%7)*2000+inputLag);
        peer->input.push_back({due,{p,p+n}});
        if(!reliable&&sends%17==0)peer->input.push_back({due+1000,{p,p+n}});
        return true;
    }
    bool SendTo(std::uint8_t,const std::uint8_t* p,std::size_t n)override{return send(p,n,false);}
    bool SendRepairTo(std::uint8_t,const std::uint8_t* p,std::size_t n)override{return send(p,n,true);}
    bool SendControl(const std::uint8_t* p,std::size_t n)override{return send(p,n,true);}
    std::size_t BufferedAmount()const override{return 0;}
    bool Poll(std::vector<std::uint8_t>* p)override{
        for(auto i=input.begin();i!=input.end();++i)if(i->due<=now){*p=std::move(i->bytes);input.erase(i);return true;}
        return false;
    }
};
void match(std::uint32_t requested,bool corrupt=false){
    Wire wires[2];wires[0].peer=&wires[1];wires[1].peer=&wires[0];
    th075::multiplayer::DelaySession a(wires[0]),b(wires[1]);
    th075::multiplayer::DelaySession* peers[]{&a,&b};
    for(unsigned side=0;side<2;++side){SessionConfig c;c.sessionId=9001;c.seed=1947;c.gameplayAbi=0x075111;c.gameId=75;c.localPlayer=side;assert(peers[side]->Begin(c,0,requested));}
    std::uint64_t endedAt=0;bool detected=false;std::array<unsigned,2> captures{};std::array<std::vector<std::array<std::uint16_t,2>>,2> consumed;
    for(std::uint64_t now=0;now<40'000'000;now+=1000){
        wires[0].now=wires[1].now=now;
        for(unsigned side=0;side<2;++side){auto& p=*peers[side];if(!p.Pump(now)){std::printf("FAIL side=%u time=%llu frame=%u: %s\n",side,(unsigned long long)now,p.Frame(),p.Error());std::fflush(stdout);if(corrupt&&std::string(p.Error()).find("desync at frame 120")!=std::string::npos){detected=true;break;}assert(false);}
            if(now%16000||p.Frame()>=1000)continue;
            if(p.NeedsCapture()){const auto bits=std::uint16_t((p.Frame()*37+side*19)&255);assert(p.Capture(bits,now));++captures[side];}
            const auto d=p.Prepare();if(!d.canAdvance)continue;
            assert(d.predictedMask==0);consumed[side].push_back({d.inputs[0].buttons,d.inputs[1].buttons});assert(p.Verify(p.Frame()*711+1947+(corrupt&&side==1&&p.Frame()==120)));assert(p.Complete(d));
        }
        if(detected)break;
        if(a.Frame()==1000&&b.Frame()==1000){endedAt=now;break;}
    }
    if(corrupt){assert(detected);std::puts("PASS: conflicting peer state stops at explicit desync");return;}
    assert(a.Frame()==1000&&b.Frame()==1000);assert(consumed[0]==consumed[1]);assert(captures[0]==1000&&captures[1]==1000);
    assert(a.Delay()==b.Delay());if(requested!=AdonisStartup::Automatic)assert(a.Delay()==requested);
    for(unsigned frame=0;frame<1000;++frame)for(unsigned side=0;side<2;++side){
        const auto expected=frame<a.Delay()?0:((frame-a.Delay())*37+side*19)&255;
        assert(consumed[0][frame][side]==expected);
    }
    for(auto now=endedAt+1000;now<endedAt+500000;now+=1000){wires[0].now=wires[1].now=now;
        assert(a.Pump(now)&&b.Pump(now));assert(a.FlushFinish(now)&&b.FlushFinish(now));}
    assert(a.CanFinish()&&b.CanFinish());std::puts("PASS: terminal ACK repair without another physical capture");
    wires[1].disconnected=true;assert(!b.Ready());
    std::printf("PASS: D=%u, 1000 exact frames, loss/reorder/duplicates, one capture per frame\n",a.Delay());
}
void rollback_match(){
 Wire wires[2];wires[0].peer=&wires[1];wires[1].peer=&wires[0];
 th075::multiplayer::DelaySession a(wires[0]),b(wires[1]);auto peers=std::array{&a,&b};
 std::array<std::vector<unsigned>,2> before;std::array<unsigned,2> state{},captures{},rollbacks{};
 std::array<std::vector<std::array<std::uint16_t,2>>,2> committed;
 for(unsigned side=0;side<2;++side){SessionConfig c;c.sessionId=9101;c.seed=1947;c.gameplayAbi=0x075222;c.gameId=75;c.localPlayer=side;assert(peers[side]->Begin(c,0,0,0,8));}
 for(std::uint64_t now=0;now<50'000'000;now+=1000){
  wires[0].now=wires[1].now=now;
  for(unsigned side=0;side<2;++side){auto& p=*peers[side];assert(p.Pump(now));
   if(p.HasRollbackRequest()){auto f=p.RollbackFrame();state[side]=before[side].at(f);before[side].resize(f);assert(p.Rewind(f));++rollbacks[side];}
   if(now%16000)continue;
   auto f=p.Frame();if(f<1000){
    if(p.NeedsCapture()){assert(p.Capture(std::uint16_t((f*37+side*19)&255),now));++captures[side];}
    auto d=p.Prepare();if(d.canAdvance){before[side].push_back(state[side]);state[side]=state[side]*1664525u+d.inputs[0].buttons*17+d.inputs[1].buttons+1013904223u;assert(p.Complete(d));}
   }
   std::array<FrameInput,MAX_PLAYERS> inputs;
   while(p.ConfirmedInputs(unsigned(committed[side].size()),&inputs)){
    const auto f=unsigned(committed[side].size());committed[side].push_back({inputs[0].buttons,inputs[1].buttons});
    assert(p.VerifyAt(f,before[side].at(f)));
   }
  }
  if(committed[0].size()==1000&&committed[1].size()==1000)break;
 }
 assert(committed[0].size()==1000&&committed[0]==committed[1]);assert(state[0]==state[1]);
 assert(captures[0]==1000&&captures[1]==1000);assert(rollbacks[0]&&rollbacks[1]);
 std::printf("PASS: rollback correction, 1000 confirmed frames, captures once, rollback counts %u/%u\n",rollbacks[0],rollbacks[1]);
}

}
void held_prediction(){
 Wire wires[2];wires[0].peer=&wires[1];wires[1].peer=&wires[0];
 th075::multiplayer::DelaySession a(wires[0]),b(wires[1]);auto peers=std::array{&a,&b};
 unsigned predicted=0,maxPending=0;
 for(unsigned side=0;side<2;++side){SessionConfig c;c.sessionId=9301;c.seed=1947;c.gameplayAbi=0x075333;c.gameId=75;c.localPlayer=side;assert(peers[side]->Begin(c,0,0,0,8));}
 for(std::uint64_t now=0;now<20'000'000;now+=1000){
  wires[0].now=wires[1].now=now;
  for(unsigned side=0;side<2;++side){auto& p=*peers[side];assert(p.Pump(now));
   if(!p.Ready())continue;
   wires[side].inputLag=80000;
   if(p.HasRollbackRequest())assert(p.Rewind(p.RollbackFrame()));
   if(now%16000||p.Frame()>=240)continue;
   if(p.NeedsCapture())assert(p.Capture(0xff,now));
   const auto d=p.Prepare();if(!d.canAdvance)continue;
   if(d.predictedMask&&p.Frame()>20){assert(d.inputs[1-side].buttons==0xff);++predicted;}
   if(p.Confirmed()!=INVALID_FRAME&&p.Frame()>p.Confirmed())maxPending=std::max(maxPending,p.Frame()-p.Confirmed());
   assert(p.Complete(d));
   // Direct rollback must not accrue the delay-mode positive-wait correction.
   const auto elapsed=p.Pace(16.);assert(elapsed>=16./1.02&&elapsed<=16./.98);
  }
  if(a.Frame()==240&&b.Frame()==240)break;
 }
 assert(a.Frame()==240&&b.Frame()==240&&predicted>100&&maxPending>=4);
 std::printf("PASS: held ABCD/directions predict across 80ms, pending=%u, predicted=%u, bounded smooth clock pacing\n",maxPending,predicted);
}
void sound_correction(){
 th075::multiplayer::RollbackAudio<int> audio;std::vector<int> played,stopped;
 const auto play=[&](int id){played.push_back(id);};const auto stop=[&](int id){stopped.push_back(id);};
 audio.Submit(100,{1,2},play);assert((played==std::vector<int>{1,2}));
 audio.Begin(100);audio.Submit(100,{},play);audio.Submit(101,{1,3},play);audio.Finish(stop);
 assert((played==std::vector<int>{1,2,3}));assert((stopped==std::vector<int>{2}));
 // A second correction must not replay matched sounds or stop a voice that
 // was also emitted just before the corrected interval.
 audio.Begin(101);audio.Submit(101,{1,3},play);audio.Finish(stop);
 assert(played.size()==3&&stopped.size()==1);
 audio.Submit(102,{3},play);audio.Begin(102);audio.Submit(102,{},play);audio.Finish(stop);
 assert(stopped.size()==1);audio.Confirm(120);audio.Reset();
 std::puts("PASS: immediate predicted sounds, correction deduplication, obsolete voice cancellation");
}
void stalled_cadence(){
 touhou::sdl::FrameCadence clock;
 clock.accumulate(1./60.);assert(clock.due()==1);
 // Five callbacks cannot advance while the peer's prediction window is full.
 for(unsigned n=0;n<5;++n)clock.accumulate(1./60.);
 assert(clock.due()==4);
 for(unsigned n=0;n<4;++n)clock.consume();
 assert(clock.due()==2);clock.consume();clock.consume();assert(clock.due()==0);
 // Budget yields retain debt too; long outages retain no more than 100 ms.
 clock.accumulate(10.);assert(clock.debt<=.1&&clock.due()==4);
 clock.reset();assert(clock.due()==0);
 // Ordinary play still consumes all scheduled ticks using its original API.
 assert(clock.advance(2./60.)==2&&clock.due()==0);
 std::puts("PASS: peer waits and budget yields retain bounded cadence debt");
}
void input_latch(){
 th075::multiplayer::InputLatch input;
 input.Key(44,true);input.Key(44,false);assert(input.TakeKey(44));assert(!input.TakeKey(44));
 input.Key(44,true);assert(input.TakeKey(44));input.Key(44,true);assert(!input.TakeKey(44));input.Key(44,false);
 input.Touch(16);input.Touch(0);assert(input.TakeTouch()==16);assert(input.TakeTouch()==0);
 input.Key(45,true);input.Touch(32);input.Clear();assert(!input.TakeKey(45)&&input.TakeTouch()==0);
 input.Key(999,true);assert(!input.TakeKey(999));
 std::puts("PASS: short key/touch taps survive peer wait and transfer once");
}
int main(){input_latch();stalled_cadence();sound_correction();held_prediction();rollback_match();match(0);match(2);match(9);match(Netplay::AdonisStartup::Automatic);match(2,true);return 0;}
