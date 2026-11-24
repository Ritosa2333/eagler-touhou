#include "game/SpellRules.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <tuple>
using Spell=th128::enemy::PhaseSpell;
unsigned checks=0,validChecks=0,rejectedChecks=0;
void require(bool ok,const char* what){if(!ok)throw std::runtime_error(what);}
auto fields(const Spell& s){return std::tuple{s.flags,s.age,s.value80,s.playerMode,s.id,s.limit,s.baseBonus};}
int main(){try{
 // Original explicit prefix: clear bits4/5; clear bit6; conditionally set bit6; clear bit7.
 // This literal table retains bit3 and ORs bits0/1/2. It is independent of the product's F0 expression.
 constexpr std::array<std::uint32_t,16> lowWant{7,7,7,7,7,7,7,7,15,15,15,15,15,15,15,15};
 constexpr std::array<std::uint32_t,4> high{0u,0x100u,0x12340000u,0xffffff00u};
 struct Mode{int value;std::uint32_t expected;};
 constexpr std::array<Mode,4> modes{{{0,0u},{1,0x40u},{-1,0x40u},{5,0x40u}}};
 for(const auto top:high)for(unsigned byte=0;byte<256;++byte)for(const auto mode:modes)for(bool extra:{false,true}){
  Spell s{top|byte,17,123u,mode.value,42,900,777};
  const bool ok=extra?th128::spell::beginExtraStage16Rank4(s,240,2280):th128::spell::begin(s,70,2280,2);
  require(ok,"valid source initializer rejected");
  const std::uint32_t expected=top|lowWant[byte%16]|mode.expected;
  if(s.flags!=expected){std::cerr<<"flags="<<(top|byte)<<" mode="<<mode.value<<" extra="<<extra<<" actual="<<s.flags<<" expected="<<expected<<'\n';throw std::runtime_error("original prefix flag oracle differs");}
  require(s.playerMode==mode.value&&s.age==0&&s.id==(extra?240:70)&&s.limit==2280,"initializer changed unrelated identity");
  require(s.baseBonus==(extra?500000:300000)&&s.value80==unsigned(s.baseBonus),"existing bonus contract changed");
  ++checks;++validChecks;
 }
 struct Invalid{bool extra;int id,limit,difficulty;};
 constexpr std::array<Invalid,10> bad{{
  {false,-1,2280,0},{false,256,2280,0},{false,70,300,0},{false,70,65536,0},{false,70,2280,-1},{false,70,2280,4},
  {true,239,2280,4},{true,250,2280,4},{true,240,300,4},{true,240,65536,4}
 }};
 for(const auto c:bad){
  Spell s{0xa5a5a5a5u,17,123u,-1,42,900,777};const auto before=fields(s);
  const bool ok=c.extra?th128::spell::beginExtraStage16Rank4(s,c.id,c.limit):th128::spell::begin(s,c.id,c.limit,c.difficulty);
  require(!ok&&fields(s)==before,"rejected initializer mutated state");++checks;++rejectedChecks;
 }
 require(validChecks==8192&&rejectedChecks==10&&checks==8202,"fixture coverage incomplete");
 std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"validFlagChecks\":"<<validChecks<<",\"invalidAtomicityChecks\":"<<rejectedChecks<<",\"constructors\":2,\"lowByteInputs\":256,\"highBitSentinels\":4,\"playerModes\":4,\"WorldStarts\":0,\"originalInstructionsExecuted\":0,\"prefixMismatchFound\":false,\"downstreamOriginalHelpersProven\":false}\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
