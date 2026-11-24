#include "game/PlayerHitHost.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
using namespace th128::player;
unsigned checks=0,failed=0,hosts=0,advances=0;
void verify(bool ok,const char* label){++checks;if(!ok)++failed;std::cout<<"{\"label\":\""<<label<<"\",\"passed\":"<<(ok?"true":"false")<<"}\n";}
std::vector<unsigned char> resource;
struct Trial {
 PlayerHitHost host;std::string error;std::vector<std::pair<int,int>> stops;unsigned effects=0,sounds=0;
 void setup(int held) {
  const hit::Timer zero{-1,0,0,0x4b6f58,1};
  HitHostSnapshot initial{{1,0,0},zero,{held-1,held,float(held),0x4b6f58,1},zero,{true,0},{0,0,0},{true,0}};
  if(!host.configure(resource.data(),resource.size(),initial,[this](int script,hit::Position,int,std::string&){if(script!=83&&script!=84)throw std::runtime_error("unexpected hit effect");++effects;return true;},[this](int id,std::string&){if(id!=2)throw std::runtime_error("unexpected hit sound");++sounds;return true;},error))throw std::runtime_error(error);
  ++hosts;
  if(host.confirmedOverlap({0,400,0},{false,0},error)!=1)throw std::runtime_error(error);
  verify(effects==33&&sounds==1&&host.snapshot().gate.stateF78==4,"real_hit_entry");
  verify(host.snapshot().stateTimer.current==0&&host.snapshot().protection.current==6&&host.animationState()->ticks==1,"real_hit_timers_and_ANM");
 }
 bool step(){++advances;return host.advanceWindowUnit({true,0,0,0},error);}
};
const std::vector<std::pair<int,int>> both{{1,31},{1,70}},first{{1,31}};
void success() {
 Trial t;t.setup(45);
 if(!t.host.setSoundOperationHost([&](int op,int id,std::string&){t.stops.emplace_back(op,id);return true;},t.error))throw std::runtime_error(t.error);
 verify(t.step(),"held_window_step_completes");
 verify(t.stops==both,"original_31_then70_stop_order");
 auto s=t.host.snapshot();verify(s.heldTimer.previous==-1&&s.heldTimer.current==0&&s.heldTimer.fractional==0,"held_timer_cleared");
 verify(s.stateTimer.current==1&&s.protection.current==5&&t.host.animationState()->ticks==2,"success_advances_once_after_stops");
 verify(t.step()&&t.stops==both,"neutral_next_tick_does_not_repeat_stops");
}
void rejection(int target,int type) {
 Trial t;t.setup(45);
 if(!t.host.setSoundOperationHost([&](int op,int id,std::string& error){t.stops.emplace_back(op,id);if(id!=target)return true;if(type==1)throw std::runtime_error("required_stop_throw");error="required_stop_reject";return type==2;},t.error))throw std::runtime_error(t.error);
 verify(!t.step(),"required_stop_failure_rejected");
 verify(t.stops==(target==31?first:both),"failure_keeps_original_stop_prefix");
 const auto s=t.host.snapshot();verify(s.heldTimer.current==0&&s.stateTimer.current==0&&s.protection.current==6&&t.host.animationState()->ticks==1,"failure_stops_before_timer_protection_ANM");
 verify(!t.error.empty(),"failure_reports_error");
 const auto calls=t.stops.size();verify(!t.step()&&t.stops.size()==calls,"sticky_failure_does_not_replay_output");
}
void noheld() {
 Trial t;t.setup(0);
 if(!t.host.setSoundOperationHost([&](int op,int id,std::string&){t.stops.emplace_back(op,id);return true;},t.error))throw std::runtime_error(t.error);
 verify(t.step()&&t.stops.empty(),"zero_held_has_no_stop");
 verify(t.host.snapshot().stateTimer.current==1&&t.host.snapshot().protection.current==5,"zero_held_normal_advance");
}
int main(int argc,char**argv){
 try {
  if(argc!=2)throw std::runtime_error("one actual pl00.anm parameter required");
  std::ifstream stream(argv[1],std::ios::binary);resource.assign(std::istreambuf_iterator<char>(stream),{});
  if(resource.empty())throw std::runtime_error("empty resource");
  success();rejection(31,0);rejection(70,0);rejection(70,1);rejection(70,2);noheld();
  if(hosts!=6||advances!=11||checks!=39)throw std::runtime_error("coverage mismatch");
  std::cout<<"{\"summary\":{\"checks\":"<<checks<<",\"failed\":"<<failed<<",\"hosts\":"<<hosts<<",\"windowCalls\":"<<advances<<",\"WorldStarts\":0,\"syntheticOutput\":true,\"originalEXEExecuted\":false}}\n";
  return failed?1:0;
 }catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 2;}
}
