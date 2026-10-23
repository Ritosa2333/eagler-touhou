import test from 'node:test';
import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {runNativeCpp} from '../../../launcher/tests/support/native-cpp.mjs';
const root=resolve(import.meta.dirname,'../../..');
const includes=[resolve(root,'ports/th10/portable/input'),resolve(root,'common/include'),resolve(root,'ports/th10/web/cpp/game')];

test('TH10 loaded-source selector binds actual SCPT references to bounded skippable MSG waits',async()=>{
  const result=await runNativeCpp(`#include "DialogueSource.hpp"
#include <cassert>
#include <iostream>
using namespace th10::adaptation;
int main(){
 std::vector<std::uint8_t> ecl(100),msg(29);
 const auto put=[](auto& b,unsigned p,unsigned value,unsigned n){for(unsigned i=0;i<n;i++)b[p+i]=(value>>(i*8))&255;};
 put(ecl,0,0x54504353,4);put(ecl,4,1,2);put(ecl,16,1,2);put(ecl,36,48,4);
 put(ecl,64,120,4);put(ecl,68,0x152,2);put(ecl,70,20,2);ecl[74]=2;ecl[75]=1;put(ecl,80,0,4);
 put(ecl,90,16,2);put(msg,0,1,4);put(msg,4,12,4);msg[14]=9;msg[15]=1;msg[16]=1;
 msg[19]=10;msg[20]=4;put(msg,21,600,4);msg[27]=0;msg[28]=0;
 const auto parse=[&]{return dialogue_sources({ecl.data(),unsigned(ecl.size())},{msg.data(),unsigned(msg.size())},2);};
 auto result=parse();assert(result.reason==0&&result.candidates.size()==1);
 const auto r=result.candidates[0];assert(r[0]==0&&r[1]==0&&r[2]==120&&r[3]==2&&r[4]==64&&r[5]==1);
 assert(r[6]==17&&r[7]==0&&r[8]==600&&r[9]==3);
 const auto before=ecl;const auto messages=msg;assert(parse().candidates==result.candidates&&ecl==before&&msg==messages);
 ecl[72]=1;assert(parse().reason==9);ecl=before;ecl[70]=15;assert(parse().reason==7);ecl=before;
 msg[16]=0;assert(parse().reason==9);msg=messages;msg[20]=250;assert(parse().reason==7);msg=messages;
 put(msg,4,500,4);assert(parse().reason==7);msg=messages;
 assert(dialogue_sources({ecl.data(),90},{msg.data(),unsigned(msg.size())},2).reason==7);
 const auto hash=SourceBytes{ecl.data(),unsigned(ecl.size())}.fnv();ecl[64]++;
 assert(SourceBytes({ecl.data(),unsigned(ecl.size())}).fnv()!=hash);
 std::cout<<"real SCPT and MSG bytes retain bounded source candidates and reject malformed references\\n";
}`,{includes});assert.match(result,/retain bounded source candidates/);
});

test('TH10 production scene gate intercepts the initial main owner and rejects identity/progression changes',async()=>{
  const result=await runNativeCpp(`#include "AdaptationState.hpp"
#include <cassert>
#include <iostream>
using namespace th10::adaptation;
int main(){
 SceneGate ordinary;ControllerIdentity main{100,200,300,300,0,0};assert(!ordinary.intercept(main,0));
 SceneGate gate;assert(!gate.arm(false));assert(gate.arm(true));assert(!gate.arm(true));gate.begin(true);
 gate.spawningMain=true;assert(gate.intercept(main,0));gate.spawningMain=false;
 assert(gate.firstBlocked&&gate.initial.owner==100&&gate.blockedCalls==1&&gate.executedCalls==0);
 for(int i=0;i<40;i++)assert(gate.intercept(main,1));assert(gate.unexpectedOwners==0);
 auto changed=main;changed.instruction++;assert(gate.intercept(changed,1));assert(gate.unexpectedOwners==1);
 SceneGate tooLate;assert(tooLate.arm(true));tooLate.begin(true);tooLate.spawningMain=true;
 assert(tooLate.intercept(main,1));assert(!tooLate.firstBlocked&&tooLate.unexpectedOwners==1);
 SceneGate replay;assert(replay.arm(true));replay.begin(false);assert(replay.status==3&&!replay.active);
 State actions;actions.publish(10,1,1,0,0,0,0,2,0,0,0,0,19,1);
 assert(actions.actions[1]==255&&actions.actions[3]==1&&actions.actions[16]==0);
 actions.bombsUsed=actions.bombsUsedStage=1;actions.publish(11,1,2,0,0,0,0,2,1,1,0,0,40,1);
 assert(actions.actions[3]==2&&actions.actions[16]==2&&actions.actions[17]==1);
 actions.hostFrames=12;actions.publish_paused(1,1);assert(actions.actions[1]==129&&actions.actions[2]==2&&actions.actions[6]==0);
 std::cout<<"native scene/action boundary passed\\n";
}`,{includes});assert.match(result,/native scene\/action boundary passed/);
});

test('TH10 disposable native file overlay preserves read/seek data and never reports a persistent commit',async()=>{
  const result=await runNativeCpp(`#include "ValidationWriteOverlay.hpp"
#include <cassert>
#include <iostream>
int main(){th10::adaptation::ValidationWriteOverlay o;unsigned out[8];
 assert(!o.lock(true));assert(o.lock(false));assert(!o.lock(false));
 assert(o.open(1,"scoreth10.dat",true));const unsigned char bytes[]{1,2,3,4};assert(o.write(1,bytes,4)==4);o.close(1);
 assert(o.has_file("scoreth10.dat")&&o.open(2,"scoreth10.dat",false));unsigned char read[4]{};
 assert(o.read(2,read,4)==4&&read[2]==3);assert(o.seek(2,-2,2)==2);assert(o.read(2,read,4)==2&&read[0]==3);
 o.receipt(out);assert(out[1]==1&&out[3]==1&&out[4]==1&&out[5]==4&&out[6]==0&&out[7]==0);
 assert(o.write(2,bytes,4)==0);o.receipt(out);assert(out[7]==1&&out[6]==0);o.reset();o.receipt(out);
 assert(out[1]==0&&out[3]==0&&!o.has_file("scoreth10.dat"));std::cout<<"native overlay passed\\n";
}`,{includes});assert.match(result,/native overlay passed/);
});

test('TH10 one held touch swipe produces one navigation edge while native keyboard repeat remains active',async()=>{
  const result=await runNativeCpp(`#include "TouchController.hpp"
#include "GameInput.hpp"
#include "GameInput.cpp"
#include <cassert>
#include <iostream>
int main(){touhou::input::TouchController touch;touhou::input::TouchState state;state.context=0;state.ready=true;
 touch.pointer(0,7,.5f,.5f,0,state,false);touch.pointer(1,7,.5f,.65f,10,state,false);
 unsigned moves=0;for(unsigned tick=0;tick<40;tick++){auto s=touch.sample(state,tick*17,false,false);if(s.keys[40])++moves;}
 assert(moves==1);touch.pointer(1,7,.5f,.5f,700,state,false);touch.pointer(2,7,.5f,.5f,710,state,false);
 assert(!touch.sample(state,720,false,false).keys[90]);
 th10::GameInput input{};unsigned repeated=0;for(unsigned tick=0;tick<40;tick++){input.update_raw(32);if(input.raw_repeat&32)++repeated;}
 assert(repeated>0);std::cout<<"swipe edge and keyboard repeat passed\\n";
}`,{includes});assert.match(result,/swipe edge and keyboard repeat passed/);
});
