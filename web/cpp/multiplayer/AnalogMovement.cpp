#include "AnalogMovement.hpp"
#include "../../../portable/input/MotionTrack.hpp"
#include <algorithm>
namespace th08::multiplayer {
bool ResolveAnalogMovement(const AnalogInput& input,TouchRemainder& remainder,const PlayerMovementState& state,
                          const Vec2& minimum,const Vec2& extent,float speed,
                          const FrameTiming& timing,float& x,float& y) noexcept {
    if(input.mode==u8(Netplay::AnalogMode::None)){remainder={};return false;}
    if(input.mode==u8(Netplay::AnalogMode::Joystick)){
        remainder={};
        x=Scalar::mul(input.x,speed);y=Scalar::mul(input.y,speed);
        touhou::input::limit_vector(x,y,speed);return true;
    }
    const bool fresh=input.mode==u8(Netplay::AnalogMode::DirectTouchDelta)||
                     input.mode==u8(Netplay::AnalogMode::DirectTouchBegin);
    if(fresh){
        if(!remainder.active||input.mode==u8(Netplay::AnalogMode::DirectTouchBegin))remainder={};
        remainder.active=1;
        remainder.x=Scalar::add(remainder.x,input.x);remainder.y=Scalar::add(remainder.y,input.y);
        // A bound cannot retain unreachable movement debt.
        const auto max_x=Scalar::add(minimum.x,extent.x),max_y=Scalar::add(minimum.y,extent.y);
        remainder.x=Scalar::sub(std::clamp(Scalar::add(state.position.x,remainder.x),minimum.x,max_x),state.position.x);
        remainder.y=Scalar::sub(std::clamp(Scalar::add(state.position.y,remainder.y),minimum.y,max_y),state.position.y);
    }else if(input.mode==u8(Netplay::AnalogMode::DirectTouch))remainder={};
    else {remainder={};return false;}
    const float dx=fresh?remainder.x:input.x,dy=fresh?remainder.y:input.y;
    const float sx=state.multiplier.x*timing.rate,sy=state.multiplier.y*timing.rate;
    x=sx?dx/sx:0;y=sy?dy/sy:0;
    if(!input.unlimited)touhou::input::limit_vector(x,y,speed);
    return true;
}
}
