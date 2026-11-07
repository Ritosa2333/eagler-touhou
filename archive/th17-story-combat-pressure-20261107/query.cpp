#include "DamageSources.hpp"
#include <cstdint>
using namespace th17;
extern "C" __attribute__((export_name("run"))) std::uint32_t run(unsigned queries,unsigned pattern){
 DamageSources world;
 unsigned rng=87635;
 auto random=[&](){rng=rng*1664525u+1013904223u;return (rng>>8)%2048;};
 for(unsigned i=0;i<176;++i){
  auto& source=world.sources[i];
  source.flags=1|(i%11==0?2:0);
  source.lifetime.current=42;source.interval=1;
  source.motion.position={float(int(random())-1024)*.25f,float(random())*.25f,0};
  source.size={4.f+float(i%35),6.f+float(i%42)};
  source.radius=5.f+float(i%27);
  source.angle=pattern==0?-1.5707964f:pattern==1?-1.5707964f+float(i%5)*.1f:float(random())*.005f;
  source.damage=int(3+i%67);source.overlap_group=int(i%4);source.damage_limit=9999999;
 }
 u32 hash=0;
 for(unsigned n=0;n<queries;++n){
  DamageQuery target;
  target.position={float(int(random())-1024)*.25f,float(random())*.25f,0};
  target.radius=5.f+float(random()%50);target.rectangle=n%13==0;
  target.size={4.f+float(random()%50),8.f+float(random()%30)};
  target.angle=n%29==0?-0.f:float(random())*.004f;
  target.preview=n%5!=0;
  const auto result=world.query(target);
  hash=hash*31+u32(result.amount)+float_to_bits(result.position.x)+float_to_bits(result.position.y)+u32(result.position_written);
 }
 return hash;
}