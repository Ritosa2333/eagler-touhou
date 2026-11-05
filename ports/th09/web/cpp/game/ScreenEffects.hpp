#pragma once
#include "Timer.hpp"
#include "Rng.hpp"
#include <vector>
namespace th09 {
struct SceneFade {
    // The two values after duration are colors for fades, amplitudes/counts
    // for shakes and flashes. Keeping their integer representation matters.
    i32 kind=0,duration=0;u32 color=0,secondary=0;i32 delay=0,draw_order=35,side=2;
};
struct ScreenEffect {
    SceneFade request;Timer time; i32 opacity=0;bool release=false,active=true;
    explicit ScreenEffect(const SceneFade& r):request(r){time.reset();}
};
struct ScreenEffectContext {
    FrameTiming timing;u32 game_flags=0,field_flags[3]{},active_frames=0;
    i32 application_transition=0;bool paused=false,game_over=false;
};
struct ScreenEffectOutput {
    virtual ~ScreenEffectOutput()=default;
    virtual void shake(i32 side,float x,float y)=0;
    virtual void rectangle(float left,float top,float right,float bottom,u32 color,bool full_view)=0;
};
class ScreenEffects {
    Rng& random;ScreenEffectOutput& output;
public:
    std::vector<ScreenEffect> effects;
    ScreenEffects(Rng& r,ScreenEffectOutput& o):random(r),output(o){}
    bool update_one(ScreenEffect&,const ScreenEffectContext&);
    void draw_one(const ScreenEffect&);
    u32 create(const SceneFade& request){effects.emplace_back(request);return u32(effects.size()-1);}
    void release(u32 handle){if(handle<effects.size()){effects[handle].release=true;effects[handle].time.reset();}}
    void update(const ScreenEffectContext& c){for(auto& e:effects)if(e.active)e.active=update_one(e,c);}
    void draw(i32 order){for(const auto& e:effects)if(e.active&&e.request.draw_order==order)draw_one(e);}
};
}
