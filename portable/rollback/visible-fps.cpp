#include "../sdl/VisibleFrameRate.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
static void check(bool ok,const char* message){if(!ok){std::fprintf(stderr,"%s\n",message);std::exit(1);}}
int main(){
    touhou::sdl::VisibleFrameRate fps;
    for(unsigned n=0;n<=144;++n)fps.sample(n*1000./144,1,0,1,true,false);
    check(fps.value()<2,"cached 144Hz redraw falsely counted as gameplay FPS");
    for(unsigned n=145;n<=288;++n)fps.sample(n*1000./144,1,0,1,true,false);
    check(fps.value()==0,"complete stall must report zero");
    fps.reset();for(unsigned n=0;n<=60;++n)fps.sample(n*1000./60,2,n,1,true,false);
    check(fps.value()>=59&&fps.value()<=61,"native 60Hz progress");
    fps.reset();for(unsigned n=0;n<=144;++n){const auto frame=unsigned(n*60./144);const auto alpha=float(n*60./144-frame);fps.sample(n*1000./144,3,frame,alpha,true,true);}
    check(fps.value()>=143&&fps.value()<=145,"real interpolated 144Hz frames");
    fps.reset();for(unsigned n=0;n<=60;++n)fps.sample(n*1000./60,4,n/3,1,true,false);
    check(fps.value()>=19&&fps.value()<=21,"severe drops must show real 20 FPS");
    for(unsigned n=61;n<=120;++n)fps.sample(n*1000./60,4,20-(n%12),1,true,false);
    check(fps.value()==0,"replayed older frames must not inflate FPS");
    std::puts("PASS: cached redraws, stalls, drops, correction and genuine 144Hz presentation");
}
