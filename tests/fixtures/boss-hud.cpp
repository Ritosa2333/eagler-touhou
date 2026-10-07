#include "../../src/game/GameState.hpp"
#include <sstream>
namespace th12 {
anm_logic::Scene animationScene;
int px=0,py=0,dialogue=0,stageNumber=1;
Enemy fixtureBoss;
bool present=true;
uint64_t serial=0;
std::vector<std::array<int,3>> events;
Enemy* primaryBoss(){return present?&fixtureBoss:nullptr;}
uint64_t bindScreenAnimation(int bank,int script,int layer){
  const auto key=++serial;
  events.push_back({0,int(key),script});
  animationScene.bind(key,0,bank,script,anm_logic::Membership::Primary,0,0,{},false,nullptr,0,0,layer);
  animationScene.customInterrupt(key,[key](int code){events.push_back({1,int(key),code});});
  return key;
}
}
extern "C" {
int load_bank(int bank,const uint8_t* p,int n){std::string error;return th12::animationScene.registry.loadBank(bank,p,n,error);}
void reset_hud(float fill,float target,int shifted){th12::animationScene.reset();th12::bossHud={};th12::bossHud.fill=fill;th12::bossHud.target=target;th12::bossHud.shifted=shifted;th12::serial=0;}
void marker(int i,float fraction,uint32_t color){th12::bossHud.markers[i]=fraction;th12::bossHud.colors[i]=color;}
void input(int health,int maximum,int stage,int checkpoint,int remaining,float x,float y,int present,int dialogue){
  th12::fixtureBoss.life=health;th12::fixtureBoss.maxLife=maximum;
  th12::stageNumber=stage;th12::bossHud.checkpoint=checkpoint;th12::bossHud.remaining=remaining;
  th12::px=int(x*128);th12::py=int(y*128);th12::present=present;th12::dialogue=dialogue;
}
void tick_hud(){th12::events.clear();th12::tickBossHud();}
const char* hud_snapshot(){
  static std::string output;std::ostringstream out;out<<"{\"hud\":"<<th12::captureBossHud()<<",\"name\":"<<th12::bossHud.name<<",\"shifted\":"<<th12::bossHud.shifted<<",\"stars\":[";
  for(int i=0;i<10;i++){if(i)out<<',';out<<th12::bossHud.stars[i];}
  out<<"],\"events\":[";bool comma=false;
  for(auto event:th12::events){if(comma)out<<',';comma=true;out<<'['<<event[0]<<','<<event[1]<<','<<event[2]<<']';}
  out<<"]}";output=out.str();return output.c_str();
}
}
