#pragma once
#include "Types.hpp"
namespace th11 {
#if PORT_TITLE==14
enum class PointAddAccessTh14:u32 {Point,MaximumCompare,MaximumCandidate,Store};
// 439d38..439d57: CMOVG reads its memory candidate regardless of selection.
template<class Read,class Write> inline void th14_add_point_words(i32 amount,Read&& read,Write&& write){
    using A=PointAddAccessTh14;
    const i32 sum=wrapping_add(read(A::Point),amount);
    const bool capped=sum>read(A::MaximumCompare);
    const i32 candidate=read(A::MaximumCandidate);
    write(A::Store,capped?candidate:sum);
}
enum class OptionPowerAccessTh14:u32 {Power,Step,Maximum};
struct OptionPowerWordsTh14 {i32 count=0;bool full=false,valid=false;};
// 44f991..44f9b5 keeps power in ESI for both division and the max comparison.
// Invalid nonpositive steps remain a port error, outside the retail SHT domain.
template<class Read> inline OptionPowerWordsTh14 th14_option_power_words(Read&& read){
    using A=OptionPowerAccessTh14;
    const i32 power=read(A::Power),step=read(A::Step);
    if(step<=0)return {};
    const i32 count=power/step;
    const i32 maximum=read(A::Maximum);
    return {count,power>=maximum,true};
}
#endif
}
