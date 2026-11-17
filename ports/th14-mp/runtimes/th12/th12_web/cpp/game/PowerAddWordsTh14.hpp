#pragma once
#include "Types.hpp"
namespace th11 {
#if PORT_TITLE==14
enum class PowerAddAccessTh14:u32 {Initial,Maximum,StoreSum,StoreClamp,Notify,Final,Step};
// 447a80 retains the initial power and maximum through the clamp. Only after
// the optional notice does it reload power and step once for both divisions.
template<class Read,class Write,class Notify> inline bool th14_add_power_words(i32 amount,bool& changed,Read&& read,Write&& write,Notify&& notify){
    using A=PowerAddAccessTh14;changed=false;
    const i32 initial=read(A::Initial),maximum=read(A::Maximum);
    if(initial>=maximum)return true;
    const i32 sum=wrapping_add(initial,amount);write(A::StoreSum,sum);
    if(sum>maximum){write(A::StoreClamp,maximum);if(!notify(A::Notify))return false;}
    const i32 final=read(A::Final),step=read(A::Step);
    // A nonpositive step is outside the live SHT domain. The port propagates
    // failure at its first required division instead of trapping the host.
    if(step<=0)return false;
    changed=wrapping_sub(final,amount)/step!=final/step;return true;
}
enum class DeathPowerAccessTh14:u32 {Step,Power,Store};
// 44dda3..44ddc5 reads the step first and keeps it through the lower clamp.
template<class Read,class Write> inline void th14_death_power_words(Read&& read,Write&& write){
    using A=DeathPowerAccessTh14;
    const i32 step=read(A::Step),power=read(A::Power);
    const i32 reduced=wrapping_sub(power,step/2);
    write(A::Store,reduced<step?step:reduced);
}
#endif
}
