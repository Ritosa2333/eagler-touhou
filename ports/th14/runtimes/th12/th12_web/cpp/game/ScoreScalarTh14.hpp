#pragma once
#include "HudScalarTh14.hpp"
#include <algorithm>

namespace th11 {
#if PORT_TITLE==14
// Retail4395fd..43962c: one score load, signed division by ten, wrapping
// addition and unsigned cap, then one store. A concurrent stage restore
// between the load and store may be overwritten, as in the original.
template<class Read,class Write>
inline void th14_add_score_words(i32 amount,Read&& read,Write&& write)noexcept{
    const i32 sum=wrapping_add(read(),amount/10);
    write(signed_bits(std::min(u32(sum),999999999u)));
}
// The spell body loads score before reading its live bonus. The damage body
// computes both signed divisions before loading score; keep those orders.
template<class Read,class Bonus,class Write>
inline void th14_spell_score_words(Read&& read,Bonus&& bonus,Write&& write)noexcept{
    const i32 before=read();const i32 amount=bonus()/10;
    write(signed_bits(std::min(u32(before)+u32(amount),999999999u)));
}
template<class Read,class Write>
inline void th14_damage_score_words(i32 damage,Read&& read,Write&& write)noexcept{
    const i32 points=wrapping_add(damage/10,10)/10;
    const i32 before=read();
    write(signed_bits(std::min(u32(before)+u32(points),999999999u)));
}
template<class Write> inline void th14_clear_score_words(Write&& write)noexcept{write(i32(0));}
#endif
}
