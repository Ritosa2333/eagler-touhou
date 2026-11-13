#include "FrameStatistics.hpp"
#include <cstdio>
#if PORT_TITLE == 14
#include "ControlStateTh14.hpp"
#endif
namespace th11 {
#if PORT_TITLE == 14
namespace {
void accumulate_frame_statistics(FrameStatistics& f){
    f.nominal.store(f.nominal.load()+60);
    f.actual.store(f.actual.load()+(f.fps>57?60:double(f.fps)));
}
template<class Window> void sample_window(FrameStatistics& f,double seconds,Window on_window,u32 ticks){
    if(seconds<f.window_start)f.window_start=seconds;
    const double elapsed=seconds-f.window_start;
    if(elapsed>=1){
        f.window_start=elapsed+f.window_start;f.fps=float(double(f.frames)/elapsed);
        f.fast_windows=f.fps>65?f.fast_windows+1:0;
        on_window();f.frames=0;
    }
    f.frames+=ticks;
}
}
void FrameStatistics::sample(double seconds,bool active,u32 ticks){
    sample_window(*this,seconds,[this,active](){if(active)accumulate_frame_statistics(*this);},ticks);
}
void FrameStatistics::sample_control(double seconds,const ControlPublicationTh14& publication,u32 ticks){
    sample_window(*this,seconds,[this,&publication](){
        // Retail42e54d reads Control after the clock/window calculation.
        if(auto* control=publication.get()){
            if((u32(control->flags)&0x14u)==0)accumulate_frame_statistics(*this);
            // 42e596 consumes bit7 on every completed window, also when blocked.
            control->flags&=~0x80u;
        }
    },ticks);
}
void FrameStatistics::sample_configured(double seconds,const ControlPublicationTh14& control,const u8& frame_skip){
    // 42e5a7 reads the live config byte after the completed-window side effects.
    sample_control(seconds,control,0);frames+=u32(frame_skip)+1;
}
#else
void FrameStatistics::sample(double seconds,bool active,u32 ticks){
    if(seconds<window_start)window_start=seconds;
    const double elapsed=seconds-window_start;
    if(elapsed>=1){
        window_start=elapsed+window_start;fps=float(double(frames)/elapsed);
        fast_windows=fps>65?fast_windows+1:0;
        if(active){
#if PORT_TITLE == 14
            nominal.store(nominal.load()+60);
            actual.store(actual.load()+(fps>57?60:double(fps)));
#else
            nominal+=60;actual+=fps>57?60:double(fps);
#endif
        }
        frames=0;
    }
    frames+=ticks;
}
#endif
float FrameStatistics::slowdown()const{
#if PORT_TITLE == 14
    return nominal.load()?float(100.-double(float(actual.load()/nominal.load()))*100.):0;
#else
    return nominal?float(100.-double(float(actual/nominal))*100.):0;
#endif
}
AsciiRequest FrameStatistics::label()const{
    char text[64];std::snprintf(text,sizeof(text),"%2.1ffps",double(fps)+.05);
    AsciiStyle style;style.font=1;style.color=fps<30?0xff5050ffu:fps<40?0xffa0a0ffu:0xffffffffu;
    return {text,{588,470,0},style};
}
}
