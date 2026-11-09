#include "sdl/FrameCadence.hpp"
#include <cassert>
#include <cstdio>
static void quantized_display(){
 touhou::sdl::FrameCadence clock{.001};unsigned ticks=0,presents=0,doubles=0;double previous=0;
 for(unsigned i=1;i<=3600;i++){
  const double now=std::round(i*(1000./60.)*10.)/10000.;
  const auto n=clock.advance(now-previous);previous=now;
  ticks+=n;presents+=n!=0;doubles+=n>1;
 }
 std::printf("quantized 60 Hz: ticks=%u presents=%u multi=%u\n",ticks,presents,doubles);
 std::fflush(stdout);assert(ticks==3600);assert(presents==3600);assert(doubles==0);
}
int main(){
 quantized_display();
 {
  // Changing refresh rates and jitter may move a tick by half an interval,
  // but must never accumulate a faster/slower simulation clock.
  const double interval=touhou::sdl::FrameCadence::interval;
  touhou::sdl::FrameCadence clock{interval/2.};double now=0;unsigned ticks=0;
  for(unsigned hz:{60u,120u,50u,90u,60u,144u,59u,60u}){
   for(unsigned i=0;i<hz*10;i++){
    const double seconds=1./hz+(i%2?.0006:-.0006);now+=seconds;
    ticks+=clock.advance(seconds);
    assert(std::abs(ticks*interval-now)<=interval/2.+1.e-8);
   }
  }
  clock.reset();assert(clock.advance(0)==0);
 }
 for(double jitter:{.0011,.002,.004,.006}){
  touhou::sdl::FrameCadence clock{touhou::sdl::FrameCadence::interval/2.};double previous=0;
  for(unsigned i=1;i<=3600;i++){
   const double now=i/60.-(i%6==1?jitter:0);
   assert(clock.advance(now-previous)==1);previous=now;
   assert(clock.advance(0)==0);
  }
 }
 for(double tolerance:{0.,.001,touhou::sdl::FrameCadence::interval/2.})for(unsigned hz:{15u,20u,30u,50u,59u,60u,75u,90u,120u,144u,165u})for(double quantum:{.0001,.001}){
  touhou::sdl::FrameCadence clock{tolerance};unsigned ticks=0;double previous=0;
  for(unsigned i=1;i<=hz*60;i++){
   const double now=std::round((double(i)/hz)/quantum)*quantum;
   ticks+=clock.advance(now-previous);previous=now;
  }
  // At 15 Hz the four-tick callback bound may leave the final tick pending.
  ticks+=clock.advance(0);
  assert(ticks==3600); // Tolerance must not turn a 50 Hz display into faster play.
 }
 for(double quantum:{.0001,.001}){
  touhou::sdl::FrameCadence clock{.001};double previous=0;
  for(unsigned i=1;i<=3600;i++){
   const double jitter=i%3==1?-.0004:i%3==2?.0004:0;
   const double now=std::round((i/60.+jitter)/quantum)*quantum;
   assert(clock.advance(now-previous)==1);previous=now;
   assert(clock.advance(0)==0);
  }
 }
 {
  touhou::sdl::FrameCadence clock{.001};
  assert(clock.advance(1./60.-.0005)==1);assert(clock.debt<0);
  assert(clock.advance(0)==0);assert(clock.advance(.0005)==0);
  clock.reset();assert(clock.advance(0)==0);
  assert(clock.advance(.1)==4);assert(clock.advance(0)==2);
 }
 for(unsigned hz:{15u,20u,30u,60u,90u,120u,144u,165u}){
  touhou::sdl::FrameCadence clock;unsigned ticks=0;
  for(unsigned i=0;i<hz*60;++i)ticks+=clock.advance(1./hz);
  assert(ticks==3600);
 }
 touhou::sdl::FrameCadence clock;
 assert(clock.advance(.1)==4);assert(clock.advance(0)==2);
 assert(clock.advance(30)==4);assert(clock.advance(0)==2);
 clock.advance(.01);clock.reset();assert(clock.advance(0)==0);
 assert(clock.advance(-1)==0);
}
