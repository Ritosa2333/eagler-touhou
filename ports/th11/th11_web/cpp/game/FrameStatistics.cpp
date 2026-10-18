#include "FrameStatistics.hpp"
#include <cstdio>
namespace th11 {
void FrameStatistics::sample(double seconds,bool active,u32 ticks){
    if(seconds<window_start)window_start=seconds;
    const double elapsed=seconds-window_start;
    if(elapsed>=1){
        window_start=elapsed+window_start;fps=float(double(frames)/elapsed);
        fast_windows=fps>65?fast_windows+1:0;
        if(active){nominal+=60;actual+=fps>57?60:double(fps);}
        frames=0;
    }
    frames+=ticks;
}
float FrameStatistics::slowdown()const{return nominal?float(100.-double(float(actual/nominal))*100.):0;}
AsciiRequest FrameStatistics::label()const{
    char text[64];std::snprintf(text,sizeof(text),"%2.1ffps",double(fps)+.05);
    AsciiStyle style;style.font=1;style.color=fps<30?0xff5050ffu:fps<40?0xffa0a0ffu:0xffffffffu;
    return {text,{588,470,0},style};
}
}
