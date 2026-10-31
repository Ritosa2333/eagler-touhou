#include "../../src/game/Laser.hpp"
#include "../../src/game/LaserCollision.hpp"
#include <algorithm>
#include <array>
using namespace th12::laser;
namespace {
Manager manager;std::array<float,32> parameters{};std::array<float,256*16+1> output{};
struct TestWorld:World {
 std::vector<std::array<int,4>> events;int unsupportedCount=0;int collisions=0;float rate=1;
 float gameRate()const override{return rate;}
 void bindAnimation(Laser& l,Role role,const AnimationBinding& b)override{events.push_back({0,int(l.state.serialId),int(role),b.script});}
 void interruptAnimation(Laser& l,Role role,int code)override{events.push_back({1,int(l.state.serialId),int(role),code});}
 bool tickAnimation(Laser& l,Role role)override{events.push_back({2,int(l.state.serialId),int(role),0});return false;}
 void retireAnimation(Laser& l,Role role)override{events.push_back({3,int(l.state.serialId),int(role),0});}
 void unsupported(uint32_t)override{++unsupportedCount;}
 Collision lineCollision(const Vec3&,float,float,float)override{++collisions;return Collision::None;}
} world;
}
extern "C" {
float* test_parameters(){return parameters.data();}
void test_reset(){manager.reset();world.events.clear();world.unsupportedCount=world.collisions=0;world.rate=1;parameters.fill(0);}
int test_spawn(int kind){
 Parameters p;p.position={parameters[0],parameters[1],parameters[2]};p.angle=parameters[3];p.initialLength=parameters[4];p.maxLength=parameters[5];p.travelDistance=parameters[6];p.width=parameters[7];p.speed=parameters[8];
 p.velocity={parameters[9],parameters[10],parameters[11]};p.angularVelocity=parameters[12];p.initialOffset=parameters[13];p.delay=int(parameters[14]);p.grow=int(parameters[15]);p.hold=int(parameters[16]);p.shrink=int(parameters[17]);p.flags=uint32_t(parameters[18]);p.nodeCount=int(parameters[19]);p.type=int(parameters[20]);p.color=int(parameters[21]);p.lookupId=int(parameters[22]);p.sound=int(parameters[23]);return manager.spawn(Kind(kind),p,world);
}
void test_tick(){manager.tick(world);}
int test_clear(int mode,float x,float y,float w,float h){if(mode==1)return manager.clearAll(world);if(mode==2)return manager.clearRectangle(world,{x,y,0},{w,h,0});if(mode==3)return manager.clearCircle(world,{x,y,0},w);return 0;}
float* test_state(){
 const auto list=manager.ordered();output[0]=float(list.size());for(size_t i=0;i<list.size();++i){const auto& s=list[i]->state;float* p=output.data()+1+i*16;p[0]=float(s.lookupId);p[1]=float(uint32_t(s.kind));p[2]=float(uint32_t(s.phase));p[3]=float(s.age.current);p[4]=s.position.x;p[5]=s.position.y;p[6]=s.position.z;p[7]=s.angle;p[8]=s.length;p[9]=s.width;p[10]=s.speed;p[11]=s.headOffset;p[12]=float(s.retirePending);p[13]=float(s.cancellable);p[14]=float(s.serialId);p[15]=float(s.nodes.size());}return output.data();
}
int test_query(int kind,float x,float y,float angle,float length,float width,float radius){MovingLaser l;l.state.position={};l.state.kind=Kind(kind);l.state.angle=angle;l.state.length=length;l.state.width=width;return l.query({x,y,0},radius);}
int test_find(int id){return manager.find(id)!=nullptr;}
int test_clear_id(int id){return manager.clearId(world,id);}
int test_events(){return int(world.events.size());}
int test_event(int i,int j){return world.events[size_t(i)][size_t(j)];}
int test_unsupported(){return world.unsupportedCount;}
void test_extension(int slot,int kind,int duration){auto* l=manager.ordered().front();l->state.parameters.program.records[size_t(slot)]={0,0,duration,0,uint32_t(kind),false};}
int test_line(float angle,float transverse,float length,float x,float y,float hitWidth,float hitHeight,int state,int invul,int flags,int dialogue){PlayerCollision p{{x,y,0},hitWidth,hitHeight,state,invul,uint32_t(flags),dialogue!=0};auto r=collidePlayerLine({},angle,transverse,length,p);return int(r.code)|(r.beginMiss?4:0)|(r.clearLaser?8:0);}
float* test_factory(int kind,float enemyX,float enemyY,float offsetX,float offsetY,float originX,float originY,int fixed,int id){
 FactoryInput input;input.config.initialLength=parameters[4];input.config.maxLength=parameters[5];input.config.travelDistance=parameters[6];input.config.width=parameters[7];input.config.delay=int(parameters[14]);input.config.grow=int(parameters[15]);input.config.hold=int(parameters[16]);input.config.shrink=int(parameters[17]);input.config.flags=uint32_t(parameters[18]);
 input.offset={offsetX,offsetY,0};input.origin={originX,originY,0};input.fixedOrigin=fixed!=0;input.angle=parameters[3];input.speed=parameters[8];input.initialOffset=parameters[13];input.type=int(parameters[20]);input.color=int(parameters[21]);input.sound=int(parameters[23]);input.transformSound=int(parameters[24]);
 const auto p=makeParameters(Kind(kind),input,{enemyX,enemyY,0},id);output.fill(0);output[0]=p.position.x;output[1]=p.position.y;output[2]=p.position.z;output[3]=p.angle;output[4]=p.initialLength;output[5]=p.maxLength;output[6]=p.travelDistance;output[7]=p.width;output[8]=p.speed;output[9]=p.velocity.x;output[10]=p.velocity.y;output[11]=p.velocity.z;output[12]=p.angularVelocity;output[13]=p.initialOffset;output[14]=float(p.delay);output[15]=float(p.grow);output[16]=float(p.hold);output[17]=float(p.shrink);output[18]=float(p.flags);output[19]=float(p.nodeCount);output[20]=float(p.type);output[21]=float(p.color);output[22]=float(p.lookupId);output[23]=float(p.sound);output[24]=float(p.transformSound);return output.data();
}
}
