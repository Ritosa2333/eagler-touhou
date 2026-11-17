#include "FramePerformance.hpp"
namespace th17 {
void FramePerformance::sample(double seconds,bool scene,u32 flags,u32 skipped)noexcept{
    if(origin>seconds)origin=seconds;const double duration=seconds-origin;
    if(duration>=1){
        origin+=duration;fps=float(double(pending)/duration);
        if(scene&&!(flags&0x14)){requested+=60;delivered+=fps>57?60:double(fps);}
        pending=0;
    }
    pending+=skipped+1;
}
}
