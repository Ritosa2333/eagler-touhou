#pragma once
#include "Rng.hpp"
#include "NativeStackTh14.hpp"
#include "Timer.hpp"
#include "LoadingWordTh14.hpp"
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
#include "ControlStateTh14.hpp"
#endif
#include <vector>
namespace th11 {
// Native effect type 1: 4489e0/448a90, update callback 448720 at priority 14.
// The shared script RNG is intentional: replacing this with cosmetic random
// numbers changes subsequent ECL decisions and invalidates original replays.
struct ScreenShake {
    i32 duration=0,start=0,end=0;
    Timer timer;
    i32 ramp=0,hold=0,fall=0;bool phased=false;
    i32 update(Rng&,Vec2& offset,bool stopped=false,u32* last_rng_return=nullptr,bool* used_rng=nullptr
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
        ,const ControlPublicationTh14* live_control=nullptr
#endif
    ) noexcept;
};
struct ScreenShakes {
    std::vector<ScreenShake> effects;
    Vec2 offset{};
    void start(i32 duration,i32 from,i32 to,const float* rate){
        ScreenShake effect{duration,from,to};effect.timer.set(0,rate);
        // 456b70 inserts before existing callbacks with the same priority.
        effects.insert(effects.begin(),effect);
    }
    void start_th14_hold(i32 amplitude,i32 ramp,i32 hold,i32 fall,const float* rate){
        ScreenShake effect;effect.start=amplitude;effect.ramp=ramp;effect.hold=hold;effect.fall=fall;effect.phased=true;effect.timer.set(0,rate);effects.insert(effects.begin(),effect);
    }
    void update(Rng& rng,bool stopped=false,Rng* visual=nullptr,NativeStackTh14* reference_stack=nullptr
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
        ,const LoadingWordTh14<u32>* live_stop=nullptr,const ControlPublicationTh14* live_control=nullptr
#endif
    ){
        for(auto it=effects.begin();it!=effects.end();){
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
            const bool stop=live_stop?u32(*live_stop)!=0:stopped;
#else
            const bool stop=stopped;
#endif
            auto& chosen=it->phased&&visual?*visual:rng;
            u32 last_rng_return=0;bool used_rng=false;const auto result=it->update(chosen,offset,stop,&last_rng_return,&used_rng
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
                ,live_control
#endif
            );
            if(reference_stack&&used_rng){
                if(it->phased)reference_stack->controller_phased_rng_leave(last_rng_return);
                else reference_stack->controller_shake_rng_leave();
            }
            if(result==7){if(reference_stack)reference_stack->controller_opaque_leave();it=effects.erase(it);}else ++it;
        }
    }
};
}
