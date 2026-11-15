#pragma once
#include "ReflectedBullet.hpp"
#include <vector>
namespace th19 {
// ECL517 -> 502e35 -> 4b9f00(mode 1, duration, from, to, ..., slot 2).
struct ScreenShakeRequest {int duration=0,from=0,to=0;};
class ScreenShake {
    struct Effect {ScreenShakeRequest request;int age=0;};
    std::vector<Effect> effects;
    ReflectionRng rng;
public:
    float stage_x=0,stage_y=0;
    void start(ScreenShakeRequest request){if(request.duration>0)effects.push_back({request,0});}
    void reset(){effects.clear();stage_x=stage_y=0;}
    void tick(){
        stage_x=stage_y=0;
        for(auto& effect:effects){
            const auto&r=effect.request;
            // 4ba250 increments the timer BEFORE interpolation and expires at duration.
            if(++effect.age>=r.duration)continue;
            float amplitude=r.from+(r.to-r.from)*float(effect.age)/r.duration;
            auto axis=[&](){auto n=rng.next()%3;return n==1?amplitude:n==2?-amplitude:0.f;};
            // Native writes, rather than adds, offsets. Later controllers win.
            stage_x=axis();stage_y=axis();
        }
        effects.erase(std::remove_if(effects.begin(),effects.end(),[](const Effect&e){return e.age>=e.request.duration;}),effects.end());
    }
};
}
