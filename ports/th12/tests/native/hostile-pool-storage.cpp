#include "game/GameState.hpp"
#include "game/HostilePool.hpp"
#ifdef TH12_MULTIPLAYER
#include "game/GameSession.hpp"
#include "game/Multiplayer.hpp"
#include "multiplayer/WorldState.hpp"
#endif
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <unordered_map>
using namespace th12;
namespace {
bool counting=false;size_t allocations=0;
uint64_t digest=1469598103934665603ull;
void require(bool value,const char* message){if(!value){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}}
void hash(uint32_t value){for(int i=0;i<4;++i){digest^=uint8_t(value>>(i*8));digest*=1099511628211ull;}}
// Dense rebuild is the former implementation, used as an independent oracle.
struct Reference {
  bullet_pool::Slots pool;
  std::array<Projectile*,bullet_pool::Slots::capacity> pointers{};
  std::unordered_map<u32,size_t> index;
  void attach(Projectile& p){if(p.physicalSlot<0||p.physicalSlot>=int(pointers.size()))return;pointers[p.physicalSlot]=&p;index[p.entityId]=p.physicalSlot;}
  void refresh(){pointers={};index.clear();pool.phases={};for(auto& p:bullets)if(!p.friendly&&p.active&&p.enemy.phase!=eb::Phase::Inactive&&p.physicalSlot>=0){attach(p);pool.phases[p.physicalSlot]=1;}}
  void release(u32 id){auto at=index.find(id);if(at==index.end())return;const auto slot=at->second;if(auto* p=pointers[slot])pool.cancelEffects[slot]=p->enemy.cancelEffect;pool.release(slot);pointers[slot]=nullptr;index.erase(at);}
  void check(){require(pool.cursor==hostilePool.cursor&&pool.phases==hostilePool.phases&&pool.cancelEffects==hostilePool.cancelEffects&&pointers==hostileSlots,"pool identity, cursor, effects and pointers match dense rebuild");}
};
Projectile& append(u32 id,int slot,bool active=true,bool friendly=false){
  Projectile p{};p.entityId=p.enemy.id=id;p.physicalSlot=slot;p.active=active;p.friendly=friendly;
  p.enemy.phase=eb::Phase::Alive;p.enemy.cancelEffect=int(id%29);bullets.push_back(p);return bullets.back();
}
void verify(){
  bullets.clear();resetHostilePool();Reference r;
  hostilePool.cursor=r.pool.cursor=1337;
  for(size_t i=0;i<hostilePool.cancelEffects.size();++i)hostilePool.cancelEffects[i]=r.pool.cancelEffects[i]=int(i%37);
  auto refresh=[&]{refreshHostilePointers();r.refresh();r.check();};
  auto release=[&](u32 id){releaseHostileBullet(id);r.release(id);r.check();};
  for(unsigned i=0;i<2000;++i)append(i?i:0xffffffffu,int(i));
  refresh();for(int n=0;n<4;++n)refresh();release(0xffffffffu);release(0xffffffffu);release(7777777);
  // Duplicate IDs and physical-slot replacement preserve the former last-write
  // rules, including synchronous births before the next full pointer refresh.
  auto& a=append(0,13);attachHostileSlot(a);r.attach(a);r.check();
  auto& b=append(0,25);attachHostileSlot(b);r.attach(b);r.check();
  auto& c=append(9000,25);attachHostileSlot(c);r.attach(c);r.check();
  refresh();release(0);release(9000);
  auto retained=bullets;const auto retainedPool=hostilePool;
  for(auto& p:bullets)if(p.entityId%3==0)p.active=false;
  bullets.erase(std::remove_if(bullets.begin(),bullets.end(),[](const auto& p){return !p.active;}),bullets.end());
  append(123456,-1);append(123457,18,false);append(123458,19,true,true);append(123459,20).enemy.phase=eb::Phase::Inactive;
  refresh();release(123456);release(123457);release(123458);release(123459);
  for(unsigned pass=0;pass<20;++pass){
    bullets.clear();for(unsigned i=0;i<500;++i)append(20000+pass*500+i,int(i));
    refresh();release(20000+pass*500);release(pass?20000+(pass-1)*500:0);
  }
  bullets=retained;hostilePool=retainedPool;r.pool=retainedPool;refresh();
  for(const auto& p:bullets){hash(p.entityId);hash(uint32_t(p.physicalSlot));}
  for(unsigned i=0;i<2000;i+=7)release(i);
  for(auto value:hostilePool.phases)hash(value);for(auto value:hostilePool.cancelEffects)hash(value);hash(hostilePool.cursor);
  bullets.clear();resetHostilePool();r=Reference{};refresh();release(0);release(0xffffffffu);
}
#ifdef TH12_MULTIPLAYER
void verifyWorld(){
  for(unsigned count:{2u,3u}){
    selectStage(1);multiplayer::Options options;options.count=count;
    require(multiplayer::begin(options),"start synthetic rollback world");
    for(unsigned i=0;i<200;++i)append(i+100,int(i));
    hostilePool.cursor=919;refreshHostilePointers();
    const auto before=multiplayer::canonicalHash();const auto pool=hostilePool;multiplayer::WorldState saved;
    bullets.clear();for(unsigned i=0;i<300;++i)append(i+10000,int(i));refreshHostilePointers();
    saved.restore();require(multiplayer::canonicalHash()==before&&hostilePool.cursor==pool.cursor&&hostilePool.phases==pool.phases&&hostilePool.cancelEffects==pool.cancelEffects,
      "rollback reconstructs derived index while retaining physical pool state");
    Reference expected;expected.pool=pool;expected.refresh();expected.check();
    for(unsigned i=100;i<300;i+=3){releaseHostileBullet(i);expected.release(i);expected.check();}
    releaseHostileBullet(10000);expected.release(10000);expected.check();
  }
}
#endif
}
void* operator new(size_t size){if(counting)++allocations;if(void* p=std::malloc(size?size:1))return p;throw std::bad_alloc();}
void* operator new[](size_t size){return ::operator new(size);}
void operator delete(void* p) noexcept{std::free(p);}
void operator delete[](void* p) noexcept{std::free(p);}
void operator delete(void* p,size_t) noexcept{std::free(p);}
void operator delete[](void* p,size_t) noexcept{std::free(p);}
int main(int argc,char** argv){
  const bool reference=argc>1&&!std::strcmp(argv[1],"--reference");verify();
#ifdef TH12_MULTIPLAYER
  verifyWorld();bullets.clear();resetHostilePool();
#endif
  for(unsigned i=0;i<1000;++i)append(i+1,int(i));refreshHostilePointers();
  allocations=0;counting=true;const auto begin=std::chrono::steady_clock::now();
  for(unsigned n=0;n<1000;++n)refreshHostilePointers();
  const double elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-begin).count();counting=false;
  if(!reference)require(!allocations,"steady hostile index rebuild has no allocations");
  std::printf("{\"refreshes\":1000,\"bullets\":1000,\"allocations\":%zu,\"elapsedMs\":%.3f,\"outputHash\":\"%016llx\"}\n",allocations,elapsed,(unsigned long long)digest);
}
