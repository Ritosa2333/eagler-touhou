#include <eagler/thprac/PracticeSpeed.hpp>
#include "../th08_web/cpp/game/PracticeCadence.hpp"
#include <cassert>
#include <cstring>
#include "../th08_web/cpp/game/PracticeUiLabels.hpp"
int main(){
 for(int locale=0;locale<3;++locale){
  const char* cpu=th08::practice_THPRAC_SELECT_CPU_COMBO[locale];
  for(int item=0;item<3;++item){assert(*cpu);cpu+=std::strlen(cpu)+1;}
  assert(!*cpu);
 }
 eagler::thprac::PracticeSpeed s;
 assert(s.interval(false,false,false,false)==1./60.);
 assert(s.interval(true,false,true,false)==1./15.);
 s.fps_replay_fast=120;assert(s.interval(true,true,true,false)==1./120.);
 s.fps_debug_acc=1;assert(s.interval(false,false,false,true)==1./120.);
 s.fps_replay_fast=1201;assert(s.interval(true,true,false,false)==1./9999.);
 for(int rate:{15,60,75,120,240,6000}){
  th08::PracticeCadence c;c.period=1./rate;unsigned count=0;
  for(int i=0;i<600;++i)count+=c.advance(1./60.);
  assert(count==unsigned(rate*10));
  c.advance(c.period*.5);c.reset();assert(c.debt==0);
  assert(c.advance(-1)==0);assert(c.advance(100)<=1024);
 }
}
