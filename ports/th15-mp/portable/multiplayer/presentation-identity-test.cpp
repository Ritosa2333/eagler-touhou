#include "../../web/cpp/sdl/FrameIdentityTable.hpp"
#include <map>
#include <random>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <string>
using namespace th15::sdl;
using K3=std::tuple<std::uintptr_t,std::uint64_t,std::uint32_t>;
using K4=std::tuple<std::uintptr_t,std::uint64_t,std::uint32_t,std::uint32_t>;
unsigned long long checks=0;
void require(bool b,const char* message){++checks;if(!b){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}}
template<class K>K key(std::uint64_t n){
 if constexpr(std::tuple_size_v<K> == 3)return {std::uintptr_t(n*0x300010u),n*0x9e3779b97f4a7c15ull,std::uint32_t(n%31)};
 else return {std::uintptr_t(n*0x300010u),n*0x9e3779b97f4a7c15ull,std::uint32_t(n%31),std::uint32_t(n%17)};
}
template<class K,class E>void exercise(){
 FrameIdentityTable<K,std::uint32_t,E> table;std::map<K,std::uint32_t> reference;
 require(table.find(K{})==nullptr,"empty table has a value");
 std::mt19937_64 rng(0x735139);
 for(unsigned epoch=0;epoch<800;++epoch){
  table.clear();reference.clear();
  require(table.find(K{})==nullptr,"clear leaked zero key");
  // Counts, hits and misses through three 8-bit epoch wraps; power-of-two
  // boundaries vary independently from the production default 32-bit epoch.
  for(unsigned op=0;op<1000;++op){auto k=key<K>(rng()%((epoch%7+1)*71));
   if(op%4==0){auto x=table.find(k);auto y=reference.find(k);require((x!=nullptr)==(y!=reference.end()),"miss mismatch");if(x)require(*x==y->second,"lookup mismatch");}
   else {require(table[k]++==reference[k]++,"occurrence mismatch");}
  }
  table[K{}]=std::numeric_limits<std::uint32_t>::max();reference[K{}]=std::numeric_limits<std::uint32_t>::max();
  require(table[K{}]++==reference[K{}]++,"max count mismatch");require(table[K{}]==reference[K{}],"unsigned wrap mismatch");
  for(const auto& [k,v]:reference)require(table.find(k)&&*table.find(k)==v,"post-frame value missing");
 }
 // Grow across many capacities without losing existing nonzero occurrence IDs.
 table.clear();reference.clear();
 for(unsigned i=0;i<20000;++i){auto k=key<K>(i);table[k]=i+9;reference[k]=i+9;}
 for(auto [k,v]:reference)require(table.find(k)&&*table.find(k)==v,"growth corrupted value");
 // The hash's pre-avalanche terms cancel for these deliberately colliding
 // keys. Inverse is computed modulo 2^32 with Newton iterations, independently
 // from the table implementation. This also works with 32-bit uintptr_t.
 std::uint32_t inv=1;for(int i=0;i<5;++i)inv*=2-0x9e3779b9u*inv;
 table.clear();reference.clear();
 for(std::uint32_t i=0;i<1024;++i){K k{};std::get<0>(k)=i;std::get<1>(k)=std::uint32_t(i*inv);table[k]=i+1;reference[k]=i+1;}
 for(auto [k,v]:reference)require(table.find(k)&&*table.find(k)==v,"collision chain corrupted value");
 for(auto [k,v]:reference){auto duplicate=table.emplace(k);require(!duplicate.second&&*duplicate.first==v,"duplicate insertion overwrote value");}
 table.clear();for(auto [k,v]:reference)require(!table.find(k),"stale collision leaked");
}
int main(){exercise<K3,std::uint8_t>();exercise<K4,std::uint8_t>();exercise<K3,std::uint32_t>();exercise<K4,std::uint32_t>();std::printf("FRAME_IDENTITY_TABLE {\"checks\":%llu,\"epochsPerType\":800,\"typePairs\":4,\"collisionKeys\":1024,\"growthKeys\":20000}\n",checks);}
