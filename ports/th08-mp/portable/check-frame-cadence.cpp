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
  // One missed display callback must not collapse high-refresh presentation.
  assert(display.advance(.03));
  assert(display.advance(1./hz));
  for(unsigned i=0;i<touhou::sdl::PresentationCadence::disable_samples-1;++i)assert(display.advance(1./60.));
  assert(!display.advance(1./60.));display.reset();assert(!display.high_refresh);
 }

  // Once a real tick has primed a high-refresh session, one late display
  // callback must keep interpolation active while cadence hysteresis still
  // classifies the display as high refresh. Only loss of eligibility resets it.
  {
   touhou::sdl::PresentationCadence display;touhou::sdl::PresentationGate gate;
   for(unsigned i=0;i<touhou::sdl::PresentationCadence::enable_samples;++i)display.advance(1./165.);
   assert(display.high_refresh);assert(gate.advance(true,true));
   assert(display.advance(touhou::sdl::FrameCadence::interval*1.1));
   assert(gate.advance(display.high_refresh,true));
   assert(display.advance(1./165.));assert(gate.advance(display.high_refresh,false));
   for(unsigned i=0;i<touhou::sdl::PresentationCadence::disable_samples;++i)display.advance(1./60.);
   assert(!display.high_refresh);assert(!gate.advance(display.high_refresh,true));
   assert(!gate.primed);
  }

 // The presentation trajectory must never move backward when every display
 // callback renders the same prev/current pair with the residual accumulator
 // alpha. A simulation tick advances both endpoints before alpha resets.
 for(unsigned hz:{90u,120u,144u,165u}){
  touhou::sdl::FrameCadence sim;unsigned logical=0;double last=0;
  for(unsigned i=0;i<hz*4;++i){
   logical+=sim.advance(1./hz);const double alpha=sim.interpolation_alpha();
   const double position=logical?double(logical-1)+alpha:0.;
   assert(position+1.e-9>=last);last=position;
  }
 }

  // Enabling interpolation between two fixed ticks must not make the visual
  // trajectory move backward. The gate waits for the next real tick after
  // activation before using residual alpha.
  for(unsigned hz:{90u,120u,144u,165u}){
   touhou::sdl::FrameCadence sim;touhou::sdl::PresentationCadence display;touhou::sdl::PresentationGate gate;unsigned logical=0;double last=0;
   for(unsigned i=0;i<hz*3;++i){
    const double dt=1./hz;display.advance(dt);const unsigned ticks=sim.advance(dt);logical+=ticks;const bool interp=gate.advance(display.high_refresh,ticks!=0);
    const double position=interp&&logical?double(logical-1)+sim.interpolation_alpha():double(logical);assert(position+1.e-9>=last);last=position;
   }
  }
}
