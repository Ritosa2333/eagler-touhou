#include "../web/cpp/game/FixedStep.hpp"
#include <cassert>
#include <initializer_list>
#include <limits>
int main(){
 for(unsigned hz:{15u,20u,30u,50u,59u,60u,75u,90u,120u,144u,165u,240u}){
  th19::FixedStep clock;int ticks=clock.advance(0);
  for(unsigned i=1;i<=hz*60;++i)ticks+=clock.advance(i*1000./hz);
  assert(ticks==3601);
 }
 th19::FixedStep clock;clock.advance(0);assert(clock.advance(100)==6);
 clock.reset();assert(clock.advance(100000)==1);assert(clock.advance(100000)==0);
 assert(clock.advance(200000)==1);assert(clock.advance(0)==1);
}
