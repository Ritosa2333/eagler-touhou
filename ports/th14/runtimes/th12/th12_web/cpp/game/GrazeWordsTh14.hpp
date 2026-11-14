#pragma once
#include "Types.hpp"
namespace th11 {
#if PORT_TITLE==14
enum class GrazeAccessTh14:u32 {GrazeRead,GrazeWrite,PointRead,MaximumCompare,MaximumCandidate,PointWrite};
// Retail450413..45044a keeps the first graze value in EAX. CMOVG reads its
// memory source even when the earlier comparison does not select that value.
// Preserve the earlier comparison while reloading the candidate maximum.
template<class Read,class Write> inline void th14_graze_words(Read&& read,Write&& write){
    using A=GrazeAccessTh14;
    const i32 graze=read(A::GrazeRead);
    if(graze<99999999)write(A::GrazeWrite,wrapping_add(graze,1));
    const i32 point=wrapping_add(read(A::PointRead),100);
    const bool capped=point>read(A::MaximumCompare);
    const i32 candidate=read(A::MaximumCandidate);
    write(A::PointWrite,capped?candidate:point);
}
#endif
}
