#include "ReimuFrame.hpp"
namespace th075 {
void advance_reimu_frame(FighterActor& a,const FighterActor& opponent,ReimuFrameState& state,FighterActionEvents& events){
    if(a.motion.velocityX==0&&i8(a.carryMomentum)>0)a.carryMomentum=0;
    const i16 action=a.animation.cursor.action;
    if(state.blockEffectSent!=1&&action>=150&&action<=158){
        auto& visual=a.effectVisual;
        visual.projection.fill(0);visual.scale[0]=visual.scale[1]=.75f;
        visual.alpha=visual.red=visual.green=visual.blue=255;
        visual.rotation={0,50,0};
        const i32 height=action<=153?94:action<=157?60:100;
        events.effect(FighterEffectSpace::Owned,998,
            float(double(a.motion.x)+i32(a.motion.facing)*57),
            float(double(a.motion.y)+height),a.motion.facing,1,a.launchParameters,0,&visual);
        state.blockEffectSent=1;
    }
    const double distance=double(a.motion.x)-double(opponent.motion.x);
    if(distance>540||distance< -540)state.farRangeFrames=60;
    if(distance>320||distance< -320){
        state.farRangeFrames=i16(u16(state.farRangeFrames)+1);
        if(state.farRangeFrames>60)state.farRangeFrames=60;
    }else state.farRangeFrames=0;
}
}
