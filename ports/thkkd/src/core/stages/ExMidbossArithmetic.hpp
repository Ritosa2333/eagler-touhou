#pragma once
#include "core/stages/ExMidbossConstants.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/stages/Stage5BossFinalArithmetic.hpp"
#include "core/math/FiniteSqrt.hpp"
namespace kaikeidou {
inline void ex_midboss_name(std::array<std::uint8_t,64>& destination,std::uint32_t address){
    const auto* source=ex_midboss_detail::spell_name(address);std::size_t i=0;
    do{destination[i]=static_cast<std::uint8_t>(source[i]);}while(source[i++]);
}
inline binary_float::Number ex_midboss_sqrt(std::uint64_t square,unsigned precision){return finite_square_root(square,precision);}
namespace ex_midboss_detail {
using stage5_boss_final_detail::compare;
using stage5_boss_final_detail::signed_word;
using stage5_boss_final_detail::remainder;
struct Arithmetic:options_motion_detail::Arithmetic{
    binary_float::Number c(std::uint32_t address)const{return q(constant(address));}
};
}
}
