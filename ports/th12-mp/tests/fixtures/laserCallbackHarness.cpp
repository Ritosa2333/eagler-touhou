#include "../../src/game/LaserEnemyCallbacks.hpp"
#include "../../src/game/AnmScene.hpp"
#include "../../src/game/BulletTargetMotion.hpp"
#include <algorithm>
#include <array>
#include <cstring>
using namespace th12::laser;
namespace {
Manager manager;
th12::anm_logic::Scene scene;
std::array<float, 32> input{};
std::array<float, 4096> output{};
std::vector<BulletLaserSource> bullets;
EnemyCallbackContext context;
EnemyCallbackResult result;
th12::eb::TargetMotion targetMotion;
th12::eb::State targetBullet;
struct TargetWorld:th12::eb::World {int unknown=0;void emit(const th12::eb::Emission&)override{};void unsupported(uint32_t)override{++unknown;}}targetWorld;
uint16_t seed=0x1234;
uint32_t calls=0,detached=1;
bool recording=false;
struct TestWorld:World {
  std::vector<CallbackShooter> emissions;
  std::vector<std::array<float,4>> events;
  int unknown=0;
  uint64_t key(const Laser& l,Role r)const{return 0xe00000000ull+uint64_t(l.state.serialId)*2+uint32_t(r);}
  uint32_t next32()override {
    if(recording)events.push_back({0,float(seed),float(calls),0});
    const uint16_t a=uint16_t((seed^0x9630)-0x6553),b=uint16_t((a<<2)|(a>>14)),c=uint16_t((b^0x9630)-0x6553);
    seed=uint16_t((c<<2)|(c>>14));calls+=2;
    return uint32_t(a)<<16|c;
  }
  void emitBullet(const th12::eb::Emission& e)override {
    CallbackShooter s;s.emission=e;s.position={e.x,e.y,0};s.fireSound=22;emissions.push_back(s);
    if(recording)events.push_back({1,float(emissions.size()-1),float(seed),float(calls)});
  }
  void sound(int id,float x)override {if(recording)events.push_back({2,float(id),x,0});}
  void bindAnimation(Laser& l,Role role,const AnimationBinding& b)override {
    scene.bind(key(l,role),l.state.serialId,0,b.script,th12::anm_logic::Membership::Embedded);
  }
  void interruptAnimation(Laser& l,Role r,int code)override{scene.interrupt(key(l,r),code);}
  bool tickAnimation(Laser& l,Role r)override{scene.tickEmbedded(key(l,r));auto* n=scene.find(key(l,r));return !n||n->vm.state().ended;}
  void retireAnimation(Laser& l,Role r)override{scene.remove(key(l,r));}
  void spawnCancelEffect(const Vec3& p,int script,float angle,float)override {
    if(recording)events.push_back({3,p.x,p.y,float(script)});
    if(auto* n=scene.bind(0x100000000ull+detached++,0,0,script,th12::anm_logic::Membership::Primary,p.x,p.y))n->vm.control().rotation=angle;
  }
  void unsupported(uint32_t)override{++unknown;}
}world;
Parameters decode(){
  Parameters p;p.position={input[0],input[1],input[2]};p.angle=input[3];p.initialLength=input[4];p.maxLength=input[5];p.travelDistance=input[6];p.width=input[7];p.speed=input[8];
  p.velocity={input[9],input[10],input[11]};p.angularVelocity=input[12];p.initialOffset=input[13];p.delay=int(input[14]);p.grow=int(input[15]);p.hold=int(input[16]);p.shrink=int(input[17]);p.flags=uint32_t(input[18]);p.nodeCount=int(input[19]);p.type=int(input[20]);p.color=int(input[21]);p.lookupId=int(input[22]);p.sound=int(input[23]);p.transformSound=int(input[24]);return p;
}
float* encode(const CallbackShooter& s){
  const auto& e=s.emission;output.fill(0);
  float* o=output.data();o[0]=e.x;o[1]=e.y;o[2]=s.position.z;o[3]=e.angle;o[4]=e.spacing;o[5]=e.speed;o[6]=e.slow;o[7]=float(e.type);o[8]=float(e.color);o[9]=float(e.count);o[10]=float(e.layers);o[11]=float(e.mode);o[12]=float(e.program.flags);o[13]=float(s.fireSound);o[14]=float(e.program.transformSound);o[15]=float(e.program.first);
  for(int i=0;i<18;i++){const auto&r=e.program.records[size_t(i)];float* p=o+16+i*6;p[0]=r.a;p[1]=r.b;p[2]=float(r.c);p[3]=float(r.d);p[4]=float(r.kind);p[5]=float(r.concurrent);}return o;
}
}
extern "C" {
float* callback_input(){return input.data();}
int callback_load(const uint8_t* p,int n){std::string error;return scene.registry.loadBank(0,p,n,error);}
void callback_reset(){
  manager.reset();scene.reset();seed=0x1234;calls=0;detached=1;recording=false;world.emissions.clear();world.events.clear();world.unknown=0;bullets.clear();context={};result={};input.fill(0);
  scene.runtime.gameRandom32=[](){return world.next32();};scene.runtime.displayRandom32=[](){return world.next32();};scene.runtime.unsupported=[](int,int,int,const char*){++world.unknown;};
}
void callback_rng(int s,int c){seed=uint16_t(s);calls=uint32_t(c);}
void callback_add_laser(int kind,float x,float y,float z,float ix,float iy,float iz,float angle,float length,float speed,int age,int phase){
  manager.spawn(Kind(kind),decode(),world);auto* l=manager.ordered().back();auto&s=l->state;s.position={x,y,z};s.initialPosition={ix,iy,iz};s.angle=angle;s.length=length;s.speed=speed;s.extensionAge.current=age;s.phase=Phase(phase);
}
void callback_add_bullet(int alive,int phase,int marker,float x,float y,float z,float angle,float speed,float ix,float iy,float iz){bullets.push_back({{x,y,z},alive!=0,th12::eb::Phase(phase),marker,angle,speed,{ix,iy,iz}});}
void callback_run(int selector,int i0,float f0,float f1){context.integers[0]=i0;context.floats[0]=f0;context.floats[1]=f1;world.emissions.clear();world.events.clear();recording=true;result=runEnemyCallback(selector,context,manager,world,bullets);recording=false;}
int callback_emissions(){return int(world.emissions.size());}
float* callback_emission(int i){return encode(world.emissions[size_t(i)]);}
float* callback_shooter(int selector){return encode(selector==3?context.shooter0:context.shooter4);}
float* callback_result(){output[0]=float(result.visited);output[1]=float(result.matched);output[2]=float(result.emitted);output[3]=float(result.spawned);output[4]=float(result.cleared);output[5]=float(result.unsupported);return output.data();}
int callback_unknown(){return world.unknown;}
int callback_seed(){return seed;}
int callback_calls(){return int(calls);}
int callback_events(){return int(world.events.size());}
float callback_event(int i,int j){return world.events[size_t(i)][size_t(j)];}
float* callback_lasers(){const auto list=manager.ordered();output[0]=float(list.size());for(size_t i=0;i<list.size();i++){const auto&s=list[i]->state;float*p=output.data()+1+i*16;p[0]=float(s.kind);p[1]=float(s.lookupId);p[2]=float(s.phase);p[3]=s.position.x;p[4]=s.position.y;p[5]=s.position.z;p[6]=s.initialPosition.x;p[7]=s.initialPosition.y;p[8]=s.initialPosition.z;p[9]=s.angle;p[10]=s.length;p[11]=s.width;p[12]=s.speed;p[13]=float(s.protection);p[14]=float(s.cursor);p[15]=float(s.parameters.program.records[0].c);}return output.data();}
int target_begin(float x,float y,float angle,float speed,float a,float b,int duration,int flags){targetBullet={};targetMotion={};targetWorld.unknown=0;targetBullet.x=x;targetBullet.y=y;targetBullet.angle=angle;targetBullet.speed=speed;th12::eb::velocity(targetBullet,speed);return beginTargetMotion(targetMotion,targetBullet,{a,b,duration,flags,0x2000000,true},targetWorld);}
int target_tick(float rate,int integrate){int ret=tickTargetMotion(targetMotion,targetBullet,rate);if(integrate){targetBullet.x=float(double(targetBullet.x)+float(double(targetBullet.vx)*rate));targetBullet.y=float(double(targetBullet.y)+float(double(targetBullet.vy)*rate));}return ret;}
void target_integrate(float rate){targetBullet.x=float(double(targetBullet.x)+float(double(targetBullet.vx)*rate));targetBullet.y=float(double(targetBullet.y)+float(double(targetBullet.vy)*rate));}
float* target_state(){output[0]=targetBullet.x;output[1]=targetBullet.y;output[2]=targetBullet.vx;output[3]=targetBullet.vy;output[4]=targetBullet.speed;output[5]=targetBullet.angle;output[6]=float(targetBullet.active);output[7]=float(targetMotion.elapsed);output[8]=targetMotion.elapsedValue;output[9]=float(targetMotion.interpolation);output[10]=targetMotion.interpolationValue;output[11]=targetMotion.targetX;output[12]=targetMotion.targetY;output[13]=float(targetWorld.unknown);return output.data();}
int callback_owner_migration(){
  callback_reset();const uint32_t owner=0x7fffffff,newOwner=42;const uint64_t oldKey=uint64_t(owner)<<8,newKey=uint64_t(newOwner)<<8;
  auto*n=scene.bind(oldKey,owner,0,4,th12::anm_logic::Membership::Primary,12,120);if(!n)return 0;
  const auto id=n->id;const auto old=n->vm.state();const auto oldCalls=calls;const auto oldSeed=seed;const auto before=scene.chain(th12::anm_logic::Membership::Primary);std::vector<uint32_t> ids;for(auto*p:before)ids.push_back(p->id);
  if(ids.size()<2||!calls||!scene.reassignEnemyOwner(owner,newOwner))return 0;
  auto*now=scene.find(newKey);if(scene.find(oldKey)||now!=n||now->id!=id||calls!=oldCalls||seed!=oldSeed||now->vm.state().clock!=old.clock||now->vm.state().ticks!=old.ticks||now->vm.state().primary!=old.primary||now->vm.state().sprite!=old.sprite)return 0;
  auto after=scene.chain(th12::anm_logic::Membership::Primary);if(after.size()!=ids.size())return 0;for(size_t i=0;i<after.size();i++)if(after[i]->id!=ids[i]||after[i]->owner!=newOwner)return 0;
  scene.bind(uint64_t(43)<<8,43,0,4);if(scene.reassignEnemyOwner(newOwner,43)||!scene.find(newKey)||scene.find(newKey)->owner!=newOwner)return 0;
  scene.removeOwner(newOwner);for(const auto&[key,node]:scene.nodes())if(node->alive&&node->owner==newOwner)return 0;return 1;
}
}
