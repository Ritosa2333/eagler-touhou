#pragma once
#include <cstdint>

namespace soew {
// Sixth-HP units represent 1, 1/2 and 1/3 exactly. Keep each target's
// fractional damage until it reaches the original whole-HP phase thresholds.
class BossDamage {
    std::uint32_t sixths_=0;
public:
    constexpr BossDamage(std::uint16_t hp=0):sixths_(hp*6u){}
    constexpr operator std::uint16_t()const{return std::uint16_t(sixths_/6u);}
    constexpr unsigned sixths()const{return sixths_;}
    constexpr void add(unsigned hp,unsigned players){
        sixths_=(sixths_+hp*(6u/players))%(65536u*6u);
    }
};
}
