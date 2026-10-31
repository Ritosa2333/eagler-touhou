#include "game/GameState.hpp"
#include "game/GameSession.hpp"
#include "game/LaserWorld.hpp"
#include "game/Multiplayer.hpp"
#include "multiplayer/WorldState.hpp"
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <new>

namespace {
using namespace th12;
using namespace th12::laser;
bool counting=false;
size_t allocations=0;
uint64_t digest=1469598103934665603ull;
void require(bool value,const char* message){
  if(!value){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}
}
void hash(const std::string& text){for(unsigned char value:text){digest^=value;digest*=1099511628211ull;}}
struct Probe : World {
  uint32_t rng=7;
  unsigned events=0;
  uint32_t next32() override {++events;return rng=rng*1664525u+1013904223u;}
  void bindAnimation(Laser&,Role,const AnimationBinding&) override {++events;}
  void retireAnimation(Laser&,Role) override {++events;}
  void spawnCancelEffect(const Vec3&,int,float,float) override {++events;}
  void pointItem(const Vec3&,int,float,float) override {++events;}
  void graze(const Vec3&) override {++events;}
  Collision lineCollision(const Vec3&,float,float,float) override {++events;return Collision::Graze;}
};
void fill(Manager& manager,unsigned count,unsigned rotate=0,unsigned nodes=64){
  manager.reset();Probe world;
  for(unsigned i=0;i<count;++i){
    Parameters p;p.position={float(int(i%9)*24-96),64.f+float(i%11),0};
    p.angle=.2f+float(i%3)*.3f;p.width=8;p.speed=1.5f;
    p.initialLength=120;p.maxLength=160;p.travelDistance=240;p.nodeCount=int(nodes);
    p.delay=5;p.grow=10;p.hold=80;p.shrink=10;p.lookupId=100+int(i);
    require(manager.spawn(Kind((i+rotate)%3),p,world)!=0,"spawn mixed lasers");
  }
}
// Include every scalar, extension record and curve node, without pointer or
// padding bytes. This same digest is produced by the pre-optimization binary.
std::string snapshot(const Manager& manager){
  std::ostringstream out;out<<std::hexfloat<<manager.serial()<<';';
  const auto number=[&](auto... value){((out<<value<<','),...);};
  const auto vec=[&](const Vec3& v){number(v.x,v.y,v.z);};
  const auto timer=[&](const Timer& t){number(t.previous,t.current,t.value);};
  const auto acceleration=[&](const Acceleration& a){timer(a.timer);number(a.duration,a.linear,a.angular);vec(a.vector);};
  for(const auto* object:manager.ordered()){
    const auto& s=object->state;const auto& p=s.parameters;
    number(s.presentationTopology,s.serialId,s.lookupId,int(s.kind),int(s.phase));
    vec(s.position);vec(s.velocity);vec(s.initialPosition);
    number(s.angle,s.length,s.width,s.speed,s.headOffset);
    timer(s.age);timer(s.grazeAge);timer(s.extensionAge);timer(s.offscreenGrace);timer(s.wait);timer(s.offscreenGate);
    number(s.offscreenGateFlags,s.cancellable,int(s.retirePending),s.protection,s.activeExtensions,s.cursor,s.mirrorSpeed,s.mirrorRemaining,s.mirrorFlags);
    vec(p.position);vec(p.velocity);
    number(p.angle,p.width,p.speed,p.initialLength,p.maxLength,p.travelDistance,p.initialOffset,p.angularVelocity,
      p.delay,p.grow,p.hold,p.shrink,p.nodeCount,p.type,p.color,p.lookupId,p.sound,p.transformSound,p.flags);
    number(p.program.first,p.program.flags,p.program.transformSound);
    for(const auto& r:p.program.records)number(r.a,r.b,r.c,r.d,r.kind,r.concurrent);
    acceleration(s.scalarAcceleration);acceleration(s.polarAcceleration);
    number(s.nodes.size());for(const auto& n:s.nodes){vec(n.position);number(n.angle,n.speed);}
  }
  return out.str();
}
void equal(const Manager& a,const Manager& b){require(snapshot(a)==snapshot(b),"complete laser values and order match");}
void verify(bool reference){
  Manager source,target;fill(source,12);fill(target,12);target=source;equal(source,target);
  auto slots=target.ordered();const auto nodes=slots[2]->state.nodes.data();
  source.ordered()[2]->state.nodes[3].position.y=245;
  require(target.ordered()[2]->state.nodes[3].position.y!=245,"checkpoint has independent curve nodes");
  const auto before=snapshot(source);target=source;equal(source,target);
  if(!reference){require(target.ordered()[2]==slots[2],"same-kind restore keeps its object");require(target.ordered()[2]->state.nodes.data()==nodes,"same-kind restore keeps curve-node capacity");}
  source=source;require(snapshot(source)==before,"self assignment preserves the full graph");
  for(unsigned count:{3u,0u,30u,256u,1u,17u})for(unsigned rotation=0;rotation<3;++rotation){
    fill(source,count,rotation,32+rotation*17);target=source;equal(source,target);
    for(auto* object:target.ordered()){
      const auto kind=object->state.kind;
      require((kind==Kind::Moving&&dynamic_cast<MovingLaser*>(object))||
        (kind==Kind::Timed&&dynamic_cast<TimedLaser*>(object))||
        (kind==Kind::Curve&&dynamic_cast<CurveLaser*>(object)),"changed kind restores matching virtual behavior");
    }
    Manager cold(source);Probe a,b;
    // Compare continuation from a reused graph against a newly constructed one.
    for(unsigned frame=0;frame<8;++frame){target.tick(a);cold.tick(b);equal(target,cold);}
    require(target.query({0,100,0},12)==cold.query({0,100,0},12),"copied collision query agrees");
    require(target.clearCircle(a,{0,100,0},48,0,true)==cold.clearCircle(b,{0,100,0},48,0,true),"copied curve cancellation agrees");
    equal(target,cold);require(a.events==b.events&&a.rng==b.rng,"continuation callbacks and RNG agree");
    hash(snapshot(target));
  }
  for(unsigned count:{2u,3u}){
    selectStage(1);multiplayer::Options options;options.count=count;options.selections={0,3,5};
    require(multiplayer::begin(options),"start synthetic multiplayer world");fill(laserManager,32);
    multiplayer::WorldState saved;const auto expected=multiplayer::canonicalHash();const auto lasers=snapshot(laserManager);
    auto retained=saved;fill(laserManager,6,1,4);saved.restore();
    require(multiplayer::canonicalHash()==expected&&snapshot(laserManager)==lasers,"world restores the complete laser graph");
    multiplayer::WorldState::clearPool();fill(laserManager,48,2,200);retained.restore();
    require(multiplayer::canonicalHash()==expected&&snapshot(laserManager)==lasers,"live checkpoint survives pool clear and changed laser kinds");
  }
}
}
void* operator new(size_t size){if(counting)++allocations;if(void* p=std::malloc(size?size:1))return p;throw std::bad_alloc();}
void* operator new[](size_t size){return ::operator new(size);}
void operator delete(void* p) noexcept{std::free(p);}
void operator delete[](void* p) noexcept{std::free(p);}
void operator delete(void* p,size_t) noexcept{std::free(p);}
void operator delete[](void* p,size_t) noexcept{std::free(p);}
int main(int argc,char** argv){
  const bool reference=argc>1&&!std::strcmp(argv[1],"--reference");verify(reference);
  Manager source,target;fill(source,192,0,192);target=source;
  allocations=0;counting=true;const auto begin=std::chrono::steady_clock::now();
  for(unsigned n=0;n<1000;++n)target=source;
  const auto elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-begin).count();
  counting=false;equal(source,target);hash(snapshot(target));
  if(!reference)require(allocations==0,"steady laser checkpoint assignment has no allocations");
  std::printf("{\"assignments\":1000,\"lasers\":192,\"curveNodes\":192,\"allocations\":%zu,\"elapsedMs\":%.3f,\"outputHash\":\"%016llx\"}\n",
    allocations,elapsed,(unsigned long long)digest);
}
