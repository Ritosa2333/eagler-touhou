// Exercise the production Scene::order with changing schedules, owner reuse and
// equal-order ties. No graphics context is required.
#include "Animation.hpp"
#include "ExeTables.hpp"
#define private public
#include "Renderer.hpp"
#undef private
#include "TextRaster.hpp"
#include <array>
#include <list>
#define private public
#include "Scene.hpp"
#undef private
#include "Scene.cpp"
#include "game/EclInterpreter.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <new>
namespace th12 { void synchronizeEnemyAnimations(Enemy&); }
using namespace th12host;
namespace { bool counting=false;size_t allocations=0;u64 digest=1469598103934665603ull;
void require_impl(bool b,int line){if(!b){std::fprintf(stderr,"presentation regression at line %d\n",line);std::abort();}}
#define require(b) require_impl((b),__LINE__)
void hash(u32 v){for(int i=0;i<4;i++){digest^=u8(v>>(i*8));digest*=1099511628211ull;}}
u32 seed=173;u32 random32(){seed^=seed<<13;seed^=seed>>17;return seed^=seed<<5;}
void prepare(Scene& s,unsigned n,unsigned scenario){
 s.poses.clear();th12::animationScheduleState.clear();
 for(unsigned i=0;i<n;i++){
  Scene::Pose p;p.id=(i*7919u)^(scenario*31337u);p.logical=i%3!=0;p.mesh_pass0=i%3==0;
  p.a.layer=i%32;p.has_priority=true;p.priority=int(i%71);p.has_order=true;p.order=int(i%73);
  s.poses.push_back(p);
  // Most native bullet VMs share one draw priority. Other callbacks interleave.
  if(i%5)th12::animationScheduleState.push_back({p.id,int(i%7?31:i%71),u32(scenario%2?random32()%n:i),i%3});
 }
 if(scenario==7&&!th12::animationScheduleState.empty()){
  th12::animationScheduleState.push_back(th12::animationScheduleState.front());
  th12::animationScheduleState.back().priority=99; // first identity wins
 }
 if(scenario==8){s.poses.front().mesh_pass0=false;s.poses.front().priority=123;} // generic fallback still orders
}
std::vector<u32> reference(Scene& s){
 struct Expected {u32 index;int priority,order,clip;};std::vector<Expected> records;
 for(u32 i=0;i<s.poses.size();i++){
  const auto& p=s.poses[i];const th12::AnimationDrawSchedule* m=nullptr;
  if(p.logical||p.mesh_pass0)for(const auto& entry:th12::animationScheduleState)if(entry.id==p.id){m=&entry;break;}
  const int priority=m?m->priority:p.mesh_pass0?layer_priority(p.a.layer):p.priority;
  const int order=m?int(m->order):int(i%73),clip=m?int(m->clip):priority<43?2:0;
  if(priority>=0){require(p.priority==priority&&p.order==order&&p.clip==clip);records.push_back({i,priority,order,clip});}
 }
 std::sort(records.begin(),records.end(),[](const Expected& a,const Expected& b){return a.priority!=b.priority?a.priority<b.priority:a.order!=b.order?a.order<b.order:a.index<b.index;});
 std::vector<u32> expected;for(const auto& r:records)expected.push_back(r.index);return expected;
}
}
void* operator new(std::size_t n){if(counting)++allocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p)noexcept{std::free(p);}void operator delete[](void* p)noexcept{std::free(p);}
void operator delete(void* p,std::size_t)noexcept{std::free(p);}void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
int main(){Renderer gpu;TextRaster raster;Banks banks;ExeTables exe;Scene s(gpu,raster,banks,exe);
 // Use production region submission with already-bound render state. This
 // keeps the regression independent of a window or a driver implementation.
 gpu.blend_=1;gpu.current=1;gpu.point_=false;gpu.wrap_u=gpu.wrap_v=true;
 Texture warning{1,256,256};Renderer::RegionOptions options;options.mode=1;options.anchor_x=1;
 gpu.region(warning,0,0,512,32,100,100,512,1.024f,0,1,{},1,options);
 require(gpu.batch.size()==6);
 require(std::abs(gpu.batch[0].x-100.5f)<.00001f);
 require(std::abs(gpu.batch[0].y-99.988f)<.00001f);
 require(std::abs(gpu.batch[2].y-101.012f)<.00001f);
 // Its center now lands on a GLES pixel center, matching native mode1
 // and the existing original-quad samples. Mode0 retains its own rounding.
 gpu.batch.clear();options.mode=0;
 gpu.region(warning,0,0,16,16,100,100,16,16,0,1,{},1,options);
 require(gpu.batch[0].x==100&&gpu.batch[0].y==92);gpu.batch.clear();
 for(unsigned scenario=0;scenario<10;scenario++){
  prepare(s,scenario==9?1:4096,scenario);s.order();const auto expected=reference(s);require(s.sorted==expected);
  for(u32 i:s.sorted){const auto& p=s.poses[i];hash(i);hash(p.priority);hash(p.order);hash(p.clip);}
 }
 prepare(s,12000,0);s.order();const auto first=s.sorted;
 allocations=0;counting=true;const auto start=std::chrono::steady_clock::now();
 for(unsigned i=0;i<1000;i++)s.order();
 const double ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();counting=false;
 require(s.sorted==first);
 // A warp of less than64px must still break extrapolation. Both departure
 // and arrival jitter use ECL300, even when the VM identity stays unchanged.
 std::array<u8,64> raw{};u32 version=7;std::memcpy(raw.data(),&version,4);std::string why;
 require(th12::animationScene.registry.loadBank(2,raw.data(),raw.size(),why));
 th12::animationScene.registry.bank(2)->scripts.resize(1);
 th12::Enemy ghost;ghost.id=44;ghost.bank=2;ghost.x=ghost.ax=100;ghost.y=ghost.ay=120;
 th12::bindEnemyAnimation(ghost,0,0);
 auto* node=th12::animationScene.find(th12::enemyAnimationKey(44,0));require(node!=nullptr);
 node->vm.control().flags=3;node->vm.control().layer=8;node->vm.control().mode=1;
 runtime_options.limit_presentation=false;
 s.capture({});require(s.current_motion.size()==1);const auto old_key=s.current_motion.front().first;const auto old_revision=s.current_motion.front().second.revision;
 std::array<u8,24> setter{};u16 size=24;std::memcpy(setter.data()+6,&size,2);
 const float x=110,y=120;std::memcpy(setter.data()+16,&x,4);std::memcpy(setter.data()+20,&y,4);
 th12::Context context(setter.data());th12::command(ghost,context,300);th12::synchronizeEnemyAnimations(ghost);
 ++th12::frame;s.capture({});require(s.current_motion.size()==1);
 const auto new_key=s.current_motion.front().first;require(new_key==old_key&&s.current_motion.front().second.revision!=old_revision);
 require(s.previous_motion.size()==1&&s.previous_motion.front().second.revision==old_revision);
 ++th12::frame;s.capture({});require(s.current_motion.front().first==new_key&&s.previous_motion.front().first==new_key&&s.current_motion.front().second.revision==s.previous_motion.front().second.revision);
 std::printf("{\"poses\":12000,\"frames\":1000,\"orderMs\":%.3f,\"allocations\":%zu,\"outputHash\":\"%016llx\"}\n",ms,allocations,(unsigned long long)digest);
}
