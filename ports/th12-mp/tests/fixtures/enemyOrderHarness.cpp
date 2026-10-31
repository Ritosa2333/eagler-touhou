#include "../../src/game/GameState.hpp"
#include <sstream>
using namespace th12;
namespace {std::string serialized;
std::string routine(const u8* pc){const u8* closest=nullptr;std::string name;for(const auto&[n,p]:program.routines)if(p<=pc&&(!closest||p>closest)){closest=p;name=n;}return name;}
}
extern "C" {
void enemy_order_reset(){enemies.clear();bullets.clear();animationScene.reset();nextEnemyId=1;oracleFixtureFlags=3;difficulty=1;character=shot=0;px=0;py=51200;dialogue=0;spell=0;unsupported={};backgroundEnemyOrigin={};}
int enemy_order_load(const u8*p,int size){program={};return program.add(p,u32(size));}
int enemy_order_load_bank(const u8*p,int size){std::string error;return animationScene.registry.loadBank(0,p,size,error);}
void enemy_order_origin(float x,float y){backgroundEnemyOrigin={x,y};}
void enemy_order_boss(){auto p=std::make_unique<Enemy>();p->id=0;p->boss=true;enemies.push_back(std::move(p));}
int enemy_order_spawn(const char* name){Enemy parent;for(size_t i=0;i<12;i++)parent.variables[i]=u32(i+1);auto*p=spawn(name,0,100,100,0,0,false,&parent);return p?int(p->id):0;}
void enemy_order_pass(){tickEnemies();}
const char* enemy_order_state(){
 std::ostringstream o;o<<"[";bool first=true;for(const auto&p:enemies){const auto&e=*p;if(e.id==0)continue;if(!first)o<<',';first=false;o<<"{\"id\":"<<e.id<<",\"position\":["<<e.x<<','<<e.y<<"],\"z\":"<<e.z<<",\"age\":"<<e.age<<",\"hpImmunity\":"<<e.immunityTicks<<",\"guard\":"<<bool(e.flags&0x20000)<<",\"flags\":"<<e.flags<<",\"privateVariables\":[";for(size_t i=0;i<12;i++){if(i)o<<',';o<<e.variables[i];}o<<"],\"contexts\":[";bool cf=true;for(const auto&c:e.contexts){if(!cf)o<<',';cf=false;o<<"{\"routine\":\""<<(c.pc?routine(c.pc):std::string{})<<"\",\"time\":"<<c.time<<"}";}o<<"],\"animation\":[";bool af=true;for(int slot=0;slot<16;slot++)if(e.animationBound[slot]){auto*n=animationScene.find(enemyAnimationKey(e.id,slot));if(!af)o<<',';af=false;if(n)o<<"["<<slot<<','<<n->id<<','<<n->owner<<','<<n->vm.state().script<<','<<n->vm.state().engineZ<<"]";else o<<"null";}o<<"]}";}o<<"]";serialized=o.str();return serialized.c_str();
}
int enemy_order_state_size(){return int(serialized.size());}
int enemy_order_unsupported(int code){return int(unsupported[size_t(code)]);}
}
