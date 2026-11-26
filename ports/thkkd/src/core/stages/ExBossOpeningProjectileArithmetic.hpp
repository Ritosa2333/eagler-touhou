// 本批复用公共精度算术与原5E4BB0缓动。
#pragma once
#include "core/stages/ExBossOpeningProjectileRecords.hpp"
#include "core/stages/ExBossOpeningProjectileConstants.hpp"
#include "core/player/OptionsMotion.hpp"
namespace kaikeidou {
inline void ex_boss_opening_projectile_ease(ex_boss_opening_projectile_detail::Record& b,std::size_t offset,std::uint64_t target,std::uint64_t divisor,unsigned bits){b.quad(offset,ease_option_value(b.quad(offset),target,divisor,bits));}
namespace ex_boss_opening_projectile_detail {
struct Arithmetic {
 unsigned precision;using Number=binary_float::Number;
 Number q(std::uint64_t n)const{return binary_float::binary64(n);}
 Number add(Number a,Number b)const{return binary_float::add(a,b,precision);}
 Number sub(Number a,Number b)const{b.negative=!b.negative;return add(a,b);}
 Number mul(Number a,Number b)const{return binary_float::multiply(a,b,precision);}
 std::uint64_t store(Number n)const{return binary_float::to_binary64(n);}
};
}
}
