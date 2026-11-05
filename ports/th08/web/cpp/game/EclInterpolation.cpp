#include "EclVm.hpp"
#include "EclNative.hpp"
#include <cmath>
namespace th08 {
// Original 0x421120/0x421180, scheduled by the ECL interpreter 0x4184b0.
// Coefficients are stored as float32 before the Hermite weighted sum.
void EclVm::interpolate(const FrameTiming& timing){
    const Vec3 initial_position=position;bool moved=false;
    if(context().native_callback>=0){if(!context().native_instruction){invalid=true;return;}if(!execute_ecl_native(*this,context().native_callback,*context().native_instruction))return;}
    for(auto& slot:context().interpolations){
        if(!slot.active)continue;
        slot.time.tick(timing);
        if(slot.time.current>=slot.duration)slot.time.set(slot.duration);
        float fraction=(slot.time.value()/Extended::from_int(slot.duration)).to_float();
        const auto t=number(fraction),one=number(1);
        if(slot.easing>=1&&slot.easing<=3){auto v=t*t;for(i32 n=1;n<slot.easing;++n)v=v*t;fraction=v.to_float();}
        else if(slot.easing>=4&&slot.easing<=6){
            // FST keeps the first inverse at extended precision while every
            // subsequent factor is read back from its rounded float32 store.
            const auto inverse=one-t,rounded=number(inverse.to_float());auto power=inverse*rounded;
            for(i32 n=4;n<slot.easing;++n)power=power*rounded;
            fraction=(one-number(power.to_float())).to_float();
        }
        float result;
        if(slot.curve!=7){
            const float a=resolve_float(slot.values[0]).to_float();const auto b=resolve_float(slot.values[1]);
            result=((b-number(a))*number(fraction)+number(a)).to_float();
        }else{
            float values[4];for(u32 n=0;n<4;++n)values[n]=resolve_float(slot.values[n]).to_float();
            const auto v=number(fraction),two=number(2);
            const float a=((v-one)*(v-one)*(two*v+one)).to_float();
            const float b=(v*v*(number(3)-two*v)).to_float();
            const float c=((one-v)*(one-v)*v).to_float();
            const float d=((v-one)*v*v).to_float();
            result=(number(a)*number(values[0])+number(b)*number(values[1])+number(c)*number(values[2])+number(d)*number(values[3])).to_float();
        }
        // The original pointer resolver writes the target field itself when
        // the target is not a writable variable. Reuse that resolver here.
        struct {EclInstruction instruction;float target;} write{{0,7,16,0,255,1},slot.target};
        auto* output=float_target(write.instruction,0);if(output==&write.target)slot.target=result;else if(output)*output=result;
        if(slot.time.current>=slot.duration)slot.active=0;
        if(slot.target==10042.f||slot.target==10043.f||slot.target==10044.f)moved=true;
    }
    if(moved){
        // Position interpolators produce this frame's velocity. The enemy
        // manager applies it later; the ECL interpreter restores the position.
        velocity.x=Scalar::sub(position.x,initial_position.x);
        velocity.y=Scalar::sub(position.y,initial_position.y);
        direction.z=Extended::from_double(std::atan2(double(velocity.y),double(velocity.x))).to_float();
        position=initial_position;
    }
}
}
