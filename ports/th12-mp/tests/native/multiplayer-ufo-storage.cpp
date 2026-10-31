#include "game/GameState.hpp"
#include "game/GameSession.hpp"
#include "game/Multiplayer.hpp"
#include "game/UfoEffects.hpp"
#include "multiplayer/WorldState.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
using namespace th12;
namespace {
bool counting=false;size_t allocations=0;
uint64_t digest=1469598103934665603ull;
void require(bool value,const char* text){if(!value){std::fprintf(stderr,"FAIL: %s\n",text);std::exit(1);}}
void hash(uint32_t value){for(int i=0;i<4;++i){digest^=uint8_t(value>>(i*8));digest*=1099511628211ull;}}
void load(anm_logic::Scene& scene){
  // Synthetic script 0 stays stopped while the effect callback owns its life.
  std::vector<uint8_t> bank(88);bank[0]=7;bank[6]=1;bank[68]=72;
  bank[72]=63;bank[74]=8;bank[80]=bank[81]=255;
  std::string error;require(scene.registry.loadBank(0,bank.data(),bank.size(),error),"synthetic effect ANM");
  scene.runtime.delta=1;scene.runtime.gameRandom32=[] {return th12::random.next32();};
}
void fill(UfoEffects& effects,anm_logic::Scene& scene,unsigned count){
  scene.reset();effects.reset();load(scene);
  for(unsigned i=0;i<count;++i)effects.summon(scene,float(i*3),80.f+float(i),0);
  scene.tickPrimary();scene.collect();
}
std::vector<uint32_t> ids(const UfoEffects& effects,const anm_logic::Scene& scene){
  std::vector<uint32_t> out;for(const auto& [id,node]:scene.nodes())if(effects.owns(id))out.push_back(id);
  std::sort(out.begin(),out.end());return out;
}
std::string state(const UfoEffects& effects,const anm_logic::Scene& scene){
  std::ostringstream out;out<<std::hexfloat;
  for(auto id:ids(effects,scene)){
    const auto& e=*effects.state(id);
    out<<e.nodeId<<','<<uint32_t(e.kind)<<','<<e.timer.previous<<','<<e.timer.current<<','<<e.timer.value<<','<<e.radius<<','<<e.heading<<','<<e.centerColor<<','<<e.rimColor<<';';
    for(float x:e.position)out<<x<<',';
    for(const auto& p:e.points)out<<p[0]<<','<<p[1]<<',';
    for(auto color:e.colors)out<<color<<',';
    const auto& node=*scene.nodes().at(id);const auto mesh=effects.draw(node);
    out<<int(mesh.primitive)<<','<<mesh.bank<<','<<mesh.sprite<<','<<mesh.layer<<','<<mesh.drawPriority<<','<<mesh.blend<<','<<mesh.owner<<','<<mesh.nodeId<<';';
    for(const auto& v:mesh.vertices)out<<v.x<<','<<v.y<<','<<v.z<<','<<v.rhw<<','<<v.argb<<','<<v.u<<','<<v.v<<';';
  }
  return out.str();
}
void verify(bool reference){
  UfoEffects source,target;anm_logic::Scene a,b;fill(source,a,4);fill(target,b,8);
  const auto owned=ids(source,a);require(owned.size()>4,"summon callbacks create trail states");
  const auto targetIds=ids(target,b);const auto* pointer=target.state(owned.front());
  const auto rng=th12::random;target=source;
  require(state(target,a)==state(source,a),"copied effect scalars and rendered meshes agree");
  for(auto id:targetIds)require(target.owns(id)==source.owns(id),"copy retires absent effect IDs");
  require(th12::random.seed==rng.seed&&th12::random.calls==rng.calls,"copy consumes no game random numbers");
  if(!reference)require(target.state(owned.front())==pointer,"same ID reuses its independent payload");
  const auto expected=state(target,a);auto retained=source;source.reset();
  require(state(target,a)==expected&&state(retained,a)==expected,"copies retain independent state after source reset");
  retained=retained;require(state(retained,a)==expected,"self assignment retains effect graph");
  source=retained;target.reset();target=source;require(state(target,a)==expected,"copy into empty target restores all states");
  UfoEffects empty;target=empty;for(auto id:owned)require(!target.owns(id),"copy empty source removes every old payload");
  for(unsigned seats:{2u,3u}){
    selectStage(1);multiplayer::Options options;options.count=seats;options.selections={0,3,5};
    require(multiplayer::begin(options),"start synthetic multiplayer world");load(animationScene);
    ufoEffects.summon(animationScene,5,110);ufoEffects.defeat(animationScene,12,90);
    for(int i=0;i<5;++i){animationScene.tickPrimary();animationScene.collect();}
    multiplayer::WorldState checkpoint;const auto before=multiplayer::canonicalHash();
    const auto continuation=[] {
      uint64_t value=1469598103934665603ull;
      for(int frame=0;frame<70;++frame){
        animationScene.tickPrimary();animationScene.collect();
        for(unsigned char c:state(ufoEffects,animationScene)){value^=c;value*=1099511628211ull;}
        value^=th12::random.seed;value*=1099511628211ull;value^=th12::random.calls;value*=1099511628211ull;
      }
      return value;
    };
    const auto trace=continuation();const auto end=multiplayer::canonicalHash();const auto endRng=th12::random;
    checkpoint.restore();require(multiplayer::canonicalHash()==before,"world restores scene callbacks and effect state");
    require(continuation()==trace&&multiplayer::canonicalHash()==end&&th12::random.seed==endRng.seed&&th12::random.calls==endRng.calls,
      "restored effects preserve birth, expiry, meshes and per-frame random output");
    hash(uint32_t(trace));hash(uint32_t(trace>>32));
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
  UfoEffects source,target;anm_logic::Scene scene;fill(source,scene,64);target=source;
  const auto count=ids(source,scene).size();allocations=0;counting=true;const auto begin=std::chrono::steady_clock::now();
  for(int i=0;i<1000;++i)target=source;
  const double elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-begin).count();counting=false;
  require(state(source,scene)==state(target,scene),"steady copy preserves every rendered effect");
  if(!reference)require(!allocations,"steady effect checkpoint copy has no allocations");
  std::printf("{\"copies\":1000,\"effects\":%zu,\"allocations\":%zu,\"elapsedMs\":%.3f,\"outputHash\":\"%016llx\"}\n",count,allocations,elapsed,(unsigned long long)digest);
}
