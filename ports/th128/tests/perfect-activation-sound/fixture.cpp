#include "game/PerfectFreezeHost.hpp"
#include <cmath>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
namespace th=th128::perfect;
unsigned checks=0,failed=0,hosts=0,steps=0;
void verify(bool ok,const char* label){++checks;if(!ok)++failed;std::cout<<"{\"label\":\""<<label<<"\",\"passed\":"<<(ok?"true":"false")<<"}\n";}
struct Pos {float x=100,y=400;};
struct MockWorld {
 Pos pos;std::optional<unsigned> lasers=0;bool fault=false;int mode=-1;std::string error;
 Pos playerAimPosition(){return pos;}
 std::optional<unsigned> laserObjectCount(){return lasers;}
 void setPhaseSpellPlayerMode(int v){mode=v;}
 void reportRuntimeFault(std::string s){fault=true;error=std::move(s);}
};
struct MockHit {struct Gate{int stateF78=1;};struct Snapshot{Gate gate;};int state=1;Snapshot snapshot(){return{{state}};}};
struct MockIce {struct Stats{unsigned busy=0,objects=0;};Stats value;Stats statistics(){return value;}};
struct MockHud {int calls=0;bool charge(int,std::string&){++calls;return true;}};
struct SyntheticSound {int identifier;float positionX;int operation;};
std::unique_ptr<th::Host> perfectHost;MockWorld worldObject;MockWorld*world=&worldObject;MockHit hit;MockHit*playerHit=&hit;MockIce ice;MockIce*iceHost=&ice;MockHud hud;MockHud*perfectHud=&hud;
bool perfectConfigured=true,perfectModeOwned=false,perfectActivatedThisTick=false;
float playerProfileRadius=256;
struct Window {bool present=true;int perfectCharge4b4d70=10000,mode3c=0;}playerWindowControl;
struct Input {unsigned holding=2,pressed=0,released=0;}playerInput;
std::vector<SyntheticSound>grazeSounds;std::string operationError;
#include "actual-perfect-activation.inc"
unsigned protects=0,effects=0;
void fresh(int charge=10000){
 ++hosts;worldObject={};hit={};ice={};hud={};playerInput={};playerWindowControl={true,charge,0};grazeSounds.clear();operationError.clear();perfectConfigured=true;perfectModeOwned=false;perfectActivatedThisTick=false;protects=effects=0;
 perfectHost=std::make_unique<th::Host>();th::Context c;c.perfectCharge=charge;c.x=100;c.y=400;c.radius=256;c.keys=2;c.systemPresent=true;c.playerState=1;c.mode=th::Mode::Normal;c.lasers=th::LaserContext::Empty;
 th::Services s;s.protect=[](int,std::string&){++protects;return true;};s.effect=[](const th::EffectRequest&,std::string&){++effects;return true;};s.freezeBullets=[](const th::BulletRequest&,th::BulletResult&r,std::string&){r.completed=true;return true;};s.shatterIce=[](std::string&){return true;};
 std::string e;if(!perfectHost->configure(c,std::move(s),e))throw std::runtime_error(e);
}
int step(){++steps;return exactProductionStep();}
int main(){try{
 fresh();verify(step()==1,"actual_production_activation_succeeds");
 verify(perfectHost->status().active&&perfectHost->status().charge==0&&protects==1&&effects==1,"real_host_commits_one_activation");
 verify(grazeSounds.size()==2&&grazeSounds[0].identifier==46&&grazeSounds[1].identifier==51&&grazeSounds[0].operation==0&&grazeSounds[1].operation==0&&grazeSounds[0].positionX==100&&grazeSounds[1].positionX==100,"ordered_positional_46_51_play_pair");
 verify(playerWindowControl.mode3c==1&&perfectModeOwned&&worldObject.mode==1&&hud.calls==1&&perfectActivatedThisTick,"existing_mode_stock_hud_caller_continues");
 verify(step()==1&&grazeSounds.size()==2&&protects==1&&effects==1,"active_owner_does_not_repeat_pair");
 fresh();playerInput.holding=0;verify(step()==1&&grazeSounds.empty()&&!perfectHost->status().active,"no_held_no_request");
 fresh();playerInput.holding=0;playerInput.pressed=2;verify(step()==1&&grazeSounds.empty()&&!perfectHost->status().active,"pressed_only_no_request");
 fresh(9999);verify(step()==1&&grazeSounds.empty()&&!perfectHost->status().active,"stock_denial_no_pair");
 fresh();hit.state=4;verify(step()==1&&grazeSounds.empty()&&!perfectHost->status().active,"state4_denial_no_pair");
 fresh();worldObject.lasers=1;verify(step()==1&&grazeSounds.empty()&&!perfectHost->status().active,"populated_laser_denial_no_pair");
 fresh();grazeSounds.resize(65534,{-1,0,0});verify(step()==1&&grazeSounds.size()==65536&&grazeSounds[65534].identifier==46&&grazeSounds[65535].identifier==51,"exact_two_slot_capacity_accepted");
 for(unsigned count:{65535u,65536u}){fresh();grazeSounds.resize(count,{-1,0,0});verify(step()==0&&worldObject.fault&&worldObject.error=="perfect activation sound journal budget"&&grazeSounds.size()==count&&perfectHost->status().active&&perfectHost->status().charge==0&&playerWindowControl.perfectCharge4b4d70==10000&&playerWindowControl.mode3c==0&&!perfectModeOwned&&hud.calls==0&&!perfectActivatedThisTick,"full_journal_failstop_after_host_commit_no_partial_pair_or_caller_tail");}
 if(checks!=13||hosts!=9||steps!=10)throw std::runtime_error("coverage mismatch");
 std::cout<<"{\"summary\":{\"checks\":"<<checks<<",\"failed\":"<<failed<<",\"hosts\":"<<hosts<<",\"activationBlockCalls\":"<<steps<<",\"WorldStarts\":0,\"syntheticCallerAndRequestSink\":true,\"originalGameplay\":false}}\n";return failed?1:0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 2;}}
