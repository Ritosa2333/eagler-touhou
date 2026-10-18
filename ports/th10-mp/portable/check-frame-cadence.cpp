#include "sdl/FrameCadence.hpp"
#include "sdl/PresentationCadence.hpp"
#include <cassert>
int main(){
 for(unsigned hz:{15u,20u,30u,60u,90u,120u,144u,165u}){
  touhou::sdl::FrameCadence clock;unsigned ticks=0;
  for(unsigned i=0;i<hz*60;++i)ticks+=clock.advance(1./hz);
  assert(ticks==(hz<60?hz:60)*60);
 }
 touhou::sdl::FrameCadence clock;
 assert(clock.interpolation_alpha()==0);
 assert(clock.advance(touhou::sdl::FrameCadence::interval*.5)==0);
 assert(clock.interpolation_alpha()>.49&&clock.interpolation_alpha()<.51);
 assert(clock.advance(touhou::sdl::FrameCadence::interval*.5)==1);
 assert(clock.interpolation_alpha()<1.e-6);
 assert(clock.advance(.1)==1);assert(clock.advance(0)==0);
 assert(clock.interpolation_alpha()<=1);
 assert(clock.advance(30)==1);assert(clock.advance(0)==0);
 assert(clock.advance(touhou::sdl::FrameCadence::interval*3.5)==1);
 assert(clock.interpolation_alpha()>.49&&clock.interpolation_alpha()<.51);
 clock.advance(.01);clock.reset();assert(clock.advance(0)==0);
 assert(clock.interpolation_alpha()==0);
 assert(clock.advance(-1)==0);

 for(unsigned hz:{15u,20u,30u,60u}){
  touhou::sdl::PresentationCadence display;
  for(unsigned i=0;i<hz*2;++i)assert(!display.advance(1./hz));
 }
 for(unsigned hz:{90u,120u,144u,165u}){
  touhou::sdl::PresentationCadence display;
  bool high=false;for(unsigned i=0;i<hz;++i)high=display.advance(1./hz);
  assert(high);
  assert(display.advance(.03));
  assert(display.advance(1./hz));
  for(unsigned i=0;i<touhou::sdl::PresentationCadence::disable_samples-1;++i)assert(display.advance(1./60.));
  assert(!display.advance(1./60.));display.reset();assert(!display.high_refresh);
 }
}
