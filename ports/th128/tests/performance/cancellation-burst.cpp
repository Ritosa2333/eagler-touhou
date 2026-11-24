#include "game/CancellationEffects.hpp"
#include <bit>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <cmath>
using th128::effects::CancellationEffects;
std::uint64_t hash=1469598103934665603ull;
void mix(std::uint32_t v){for(int i=0;i<4;i++){hash^=(v>>(i*8))&255;hash*=1099511628211ull;}}
void fp(float f){mix(std::bit_cast<std::uint32_t>(f));}
void view(CancellationEffects& host){
 mix(host.rootCount());mix(host.nodeCount());
 for(const auto& row:host.snapshots()){
  mix(row.handle);mix(row.node);mix(row.parent);mix(unsigned(row.membership));
  const auto& s=row.state;mix(s.script);mix(s.ticks);mix(s.mode);mix(s.layer);mix(s.visible);mix(s.sprite);mix(s.spriteBank);
  mix(s.flags);mix(s.flags2);mix(s.primary);mix(s.secondary);mix(s.ended);mix(s.deleteRequested);
  for(float f:{s.x,s.y,s.z,s.rx,s.ry,s.rotation,s.sx,s.sy,s.u,s.v,s.uvScaleX,s.uvScaleY,s.engineX,s.engineY,s.engineZ,
   s.offsetX,s.offsetY,s.offsetZ,s.spriteWidth,s.spriteHeight,row.world.x,row.world.y,row.world.z,row.world.sx,row.world.sy,row.world.rotation})fp(f);
  for(auto v:s.integers)mix(v);for(auto v:s.floats)fp(v);
 }
}
int main(){
 std::ifstream file("/bullet.anm",std::ios::binary);std::vector<unsigned char> raw{std::istreambuf_iterator<char>(file),{}};
 if(raw.empty())return 2;std::string error;CancellationEffects host;
 if(!host.load(raw.data(),raw.size(),error)){std::cerr<<error;return 3;}
 std::uint32_t rng=0x31415926u;unsigned calls=0;
 host.setGameRandom([&](){calls++;rng=rng*1664525u+1013904223u;return rng;});
 auto t=std::chrono::steady_clock::now();
 for(int n=0;n<1024;n++){
  auto id=host.spawn(375,320+(n%17)-8,160+(n%23)-11,0,23,error);
  if(!id||!error.empty()){std::cerr<<n<<":"<<error;return 4;}
 }
 auto end=std::chrono::steady_clock::now();const auto birth=std::chrono::duration<double,std::milli>(end-t).count();
 view(host);mix(calls);mix(rng);
 std::cout<<"BIRTH "<<host.rootCount()<<" "<<host.nodeCount()<<" "<<calls<<" "<<rng<<" "<<hash<<"\n";
 for(int tick=0;tick<45;tick++){if(!host.tickPrimary(error)){std::cerr<<error;return 5;}view(host);mix(calls);mix(rng);}
 std::cout<<"TICKS "<<host.rootCount()<<" "<<host.nodeCount()<<" "<<calls<<" "<<rng<<" "<<hash<<"\n";
 for(int n=0;n<80;n++){
  const int script=n%3==0?355:n%3==1?357:375;
  auto id=host.spawn(script,100+n,200,0,23,error);if(!id){std::cerr<<error;return 6;}
  if(n%4==0)host.setRootPositionInterpolation(*id,110,210,0,12,3,error);
  if(n%7==0){if(!host.tickPrimary(error))return 7;}
  if(n%9==0)host.remove(*id);
  view(host);mix(calls);mix(rng);
 }
 std::cout<<"MIX "<<host.rootCount()<<" "<<host.nodeCount()<<" "<<calls<<" "<<rng<<" "<<hash<<"\n";
 if(!host.clear())return 8;view(host);
 std::cout<<"CLEAR "<<host.rootCount()<<" "<<host.nodeCount()<<" "<<calls<<" "<<rng<<" "<<hash<<"\n";
 std::cerr<<"birthMilliseconds "<<birth<<"\n";return 0;
}
