#include "game/PerfectFreezeHost.hpp"
#include <array>
#include <functional>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
using namespace th128::perfect;
unsigned checks=0,failed=0,hosts=0,activations=0;
void verify(bool ok,const char*label){++checks;if(!ok)++failed;std::cout<<"{\"label\":\""<<label<<"\",\"passed\":"<<(ok?"true":"false")<<"}\n";}
struct Trial {
 Host host;Context input;Services services;std::string error;
 unsigned protects=0,effects=0;int rejectStage=0,style=0;
 Trial(int charge=10000){
  ++hosts;input.perfectCharge=charge;input.x=0;input.y=400;input.radius=256;input.keys=2;input.xPressed=false;input.systemPresent=true;input.systemMode3c=0;input.busy=0;input.playerState=1;input.mode=Mode::Normal;input.lasers=LaserContext::Empty;input.laserCount=0;
  services.freezeBullets=[](const BulletRequest&,BulletResult&r,std::string&){r.completed=true;return true;};
  services.effect=[this](const EffectRequest&,std::string&e){++effects;return answer(2,e);};
  services.protect=[this](int ticks,std::string&e){if(ticks!=200)throw std::runtime_error("unexpectedinitialprotection");++protects;return answer(1,e);};
  services.shatterIce=[](std::string&){return true;};
 }
 bool answer(int stage,std::string&e){if(stage!=rejectStage)return true;if(style==1)throw std::runtime_error("service_throw");e="service_reject";return style==2;}
 void ready(){if(!host.configure(input,services,error))throw std::runtime_error(error);}
 bool activate(Context v){++activations;return host.activate(v,error);}
 bool idle(int charge=10000){const auto s=host.status();return !s.active&&!s.failed&&s.charge==charge&&protects==0&&effects==0;}
};
int main(){try{
 for(bool pressed:{false,true}){
  Trial t;t.ready();t.input.xPressed=pressed;
  verify(t.activate(t.input),"held_gate_accepts_regardless_press_edge");
  verify(t.host.status().active&&t.host.status().charge==0&&t.host.status().phase==0&&t.host.status().protectionTicks==200,"one_real_activation_spends_stock");
  verify(t.protects==1&&t.effects==1,"existing_protect_effect_outputs");
  verify(!t.activate(t.input)&&t.host.status().charge==0&&t.protects==1&&t.effects==1,"active_owner_prevents_duplicate");
 }
 {Trial t;t.ready();auto v=t.input;v.keys=0;v.xPressed=true;verify(!t.activate(v)&&t.idle(),"pressed_without_held_rejected");}
 using Mutate=std::function<void(Context&)>;
 const std::array<std::pair<const char*,Mutate>,12> gates{{
  {"deathbomb_domain_retained",[](Context&c){c.mode=Mode::DeathbombWindow;}},
  {"unknown_domain_retained",[](Context&c){c.mode=Mode::Unknown;}},
  {"system_absent_rejected",[](Context&c){c.systemPresent=false;}},
  {"mode_nonzero_rejected",[](Context&c){c.systemMode3c=1;}},
  {"busy_rejected",[](Context&c){c.busy=1;}},
  {"state0_rejected",[](Context&c){c.playerState=0;}},
  {"state4_rejected",[](Context&c){c.playerState=4;}},
  {"invalid_pose_rejected",[](Context&c){c.x=std::numeric_limits<float>::quiet_NaN();}},
  {"zero_radius_rejected",[](Context&c){c.radius=0;}},
  {"unknown_lasers_rejected",[](Context&c){c.lasers=LaserContext::Unknown;}},
  {"populated_lasers_rejected",[](Context&c){c.lasers=LaserContext::Populated;c.laserCount=2;}},
  {"stale_charge_rejected",[](Context&c){c.perfectCharge=9999;}}
 }};
 for(const auto&[label,change]:gates){Trial t;t.ready();auto v=t.input;change(v);verify(!t.activate(v)&&t.idle(),label);}
 for(int charge:{0,9999,30000}){Trial t(charge);t.ready();const auto ok=t.activate(t.input);verify(charge==30000?(ok&&t.host.status().charge==20000&&t.protects==1&&t.effects==1):(!ok&&t.idle(charge)),"charge_boundary_preserved");}
 for(int charge:{-1,30001}){Trial t(charge);verify(!t.host.configure(t.input,t.services,t.error)&&!t.host.status().configured&&t.protects==0&&t.effects==0,"invalid_configuration_rejected");}
 {Trial t(9999);t.ready();verify(!t.activate(t.input)&&t.idle(9999),"below_stock_first_attempt");verify(t.host.setCharge(10000,t.error),"existing_component_charge_update");t.input.perfectCharge=10000;verify(t.activate(t.input)&&t.host.status().charge==0,"held_retries_after_charge_without_new_edge");}
 for(int stage:{1,2})for(int style:{0,1,2}){Trial t;t.rejectStage=stage;t.style=style;t.ready();verify(!t.activate(t.input)&&t.host.status().failed&&!t.host.status().active&&t.host.status().charge==10000,"required_service_failure_not_committed");const auto p=t.protects,e=t.effects;verify(!t.activate(t.input)&&p==t.protects&&e==t.effects,"failed_owner_does_not_repeat_outputs");}
 for(int missing:{0,1,2}){Trial t;if(missing==0)t.services.freezeBullets={};if(missing==1)t.services.protect={};if(missing==2)t.services.shatterIce={};verify(!t.host.configure(t.input,t.services,t.error)&&!t.host.status().configured,"required_service_configuration_retained");}
 if(checks!=44||hosts!=30||activations!=34)throw std::runtime_error("coverage mismatch");
 std::cout<<"{\"summary\":{\"checks\":"<<checks<<",\"failed\":"<<failed<<",\"hosts\":"<<hosts<<",\"activationCalls\":"<<activations<<",\"WorldStarts\":0,\"syntheticServices\":true,\"originalGameplay\":false}}\n";
 return failed?1:0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 2;}}
