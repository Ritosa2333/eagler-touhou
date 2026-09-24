#include "AnalogMovement.hpp"
#include "../../../portable/input/MotionTrack.hpp"
namespace th08::multiplayer {
bool ResolveAnalogMovement(const AnalogInput& input,const PlayerMovementState& state,
                          float speed,const FrameTiming& timing,float& x,float& y) noexcept {
    if(input.mode==u8(Netplay::AnalogMode::None))return false;
    if(input.mode==u8(Netplay::AnalogMode::Joystick)){
        x=Scalar::mul(input.x,speed);y=Scalar::mul(input.y,speed);
        touhou::input::limit_vector(x,y,speed);return true;
    }
    if(input.mode!=u8(Netplay::AnalogMode::DirectTouch))return false;
    // Shared DirectTouch packets carry this frame's desired displacement, NOT
    // an absolute target: common predicts zero displacement after the first
    // missing frame. Convert to TH08's authored pre-timescale velocity here.
    const float sx=state.multiplier.x*timing.rate,sy=state.multiplier.y*timing.rate;
    x=sx?input.x/sx:0;y=sy?input.y/sy:0;
    if(!input.unlimited)touhou::input::limit_vector(x,y,speed);
    return true;
}
}
