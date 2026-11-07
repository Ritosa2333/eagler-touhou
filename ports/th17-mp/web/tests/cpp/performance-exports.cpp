#include "../../cpp/game/FramePerformance.hpp"
using namespace th17;
static FramePerformance value;
extern "C" {
__attribute__((export_name("reset"))) void reset(){value={};}
__attribute__((export_name("sample"))) void sample(double seconds,u32 scene,u32 flags,u32 skipped){value.sample(seconds,scene!=0,flags,skipped);}
__attribute__((export_name("fps"))) float fps(){return value.fps;}
__attribute__((export_name("delivered"))) double delivered(){return value.delivered;}
__attribute__((export_name("requested"))) double requested(){return value.requested;}
}
