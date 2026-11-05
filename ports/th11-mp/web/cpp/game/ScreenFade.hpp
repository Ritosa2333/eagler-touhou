#pragma once
#include "Timer.hpp"
#include <vector>
namespace th11 {
class AnmRenderer;
// Native type 2/3 playfield fades and type 5 full-screen ending fade.
struct ScreenFade {
    i32 type=2,duration=0,alpha=0;
    u32 color=0,priority=10;
    Timer timer;
    i32 update(bool stopped=false,bool frozen=false) noexcept;
    void draw(AnmRenderer&) const;
};
struct ScreenFades {
    std::vector<ScreenFade> effects;
    void start(i32 type,i32 duration,u32 color,u32 priority,const float* rate){
        ScreenFade effect;effect.type=type;effect.duration=duration;effect.alpha=type==3?255:0;effect.color=color;effect.priority=priority;effect.timer.set(0,rate);
        effects.insert(effects.begin(),effect);
    }
    void update(bool stopped=false,bool frozen=false){for(auto it=effects.begin();it!=effects.end();)if(it->update(stopped,frozen)==7)it=effects.erase(it);else ++it;}
    void draw(AnmRenderer&,u32 priority) const;
};
}
