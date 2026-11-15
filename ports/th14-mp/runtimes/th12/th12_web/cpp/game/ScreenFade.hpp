#pragma once
#include "Timer.hpp"
#include "LoadingWordTh14.hpp"
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
#include "ControlStateTh14.hpp"
#endif
#include <vector>
namespace th11 {
class AnmRenderer;
// Native type 2/3 playfield fades and type 5 full-screen ending fade.
struct ScreenFade {
    i32 type=2,duration=0,alpha=0;
    u32 color=0,priority=10;
    Timer timer;
    i32 update(bool stopped=false,bool frozen=false
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
        ,const ControlPublicationTh14* live_control=nullptr
#endif
    ) noexcept;
    void draw(AnmRenderer&) const;
};
struct ScreenFades {
    std::vector<ScreenFade> effects;
    void start(i32 type,i32 duration,u32 color,u32 priority,const float* rate){
        ScreenFade effect;effect.type=type;effect.duration=duration;effect.alpha=type==3?255:0;effect.color=color;effect.priority=priority;effect.timer.set(0,rate);
        effects.insert(effects.begin(),effect);
    }
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    void update(bool stopped=false,bool frozen=false,const LoadingWordTh14<u32>* live_stop=nullptr,const ControlPublicationTh14* live_control=nullptr){
        for(auto it=effects.begin();it!=effects.end();){
            const bool stop=live_stop?u32(*live_stop)!=0:stopped;
            if(it->update(stop,frozen,live_control)==7)it=effects.erase(it);else ++it;
        }
    }
#else
    void update(bool stopped=false,bool frozen=false){for(auto it=effects.begin();it!=effects.end();)if(it->update(stopped,frozen)==7)it=effects.erase(it);else ++it;}
#endif
    void draw(AnmRenderer&,u32 priority) const;
};
}
