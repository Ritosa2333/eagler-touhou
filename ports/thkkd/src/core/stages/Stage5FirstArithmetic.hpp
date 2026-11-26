// 五面首段逐指令精度算术与有符号整数除法。
#pragma once
#include "core/stages/Stage5FirstConstants.hpp"
#include "core/math/FiniteDivide.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/entities/EnemyPool.hpp"
namespace kaikeidou {
inline std::uint32_t stage5_first_pick_integer(const std::uint32_t& d,const std::array<std::uint32_t,6>& v){return v[d<6?d:3];}
inline std::uint64_t stage5_first_pick_real(const std::uint32_t& d,const std::array<std::uint64_t,6>& v){return v[d<6?d:3];}
inline void stage5_first_ease(Enemy& e,std::size_t offset,std::uint64_t target,std::uint64_t divisor,unsigned bits){e.quad(offset,ease_option_value(e.quad(offset),target,divisor,bits));}
namespace stage5_first_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:std::int64_t(v);}
inline std::int64_t remainder(std::uint32_t a,std::uint32_t b){if(!b)throw std::domain_error("Original stage5 IDIV zero divisor");if(a==0x80000000u&&b==0xffffffffu)throw std::domain_error("Original stage5 IDIV overflow");return signed_word(a)%signed_word(b);}
struct Arithmetic {using Number=binary_float::Number;unsigned precision;
 Number q(std::uint64_t v)const{return binary_float::binary64(v);}Number c(std::uint32_t a)const{return q(constant(a));}Number i(std::uint32_t v)const{return binary_float::integer(v);}
 Number add(Number a,Number b)const{return binary_float::add(a,b,precision);}Number sub(Number a,Number b)const{b.negative=!b.negative;return add(a,b);}Number mul(Number a,Number b)const{return binary_float::multiply(a,b,precision);}Number div(Number a,Number b)const{return binary_float::divide(a,b,precision);}std::uint64_t store(Number n)const{return binary_float::to_binary64(n);}
};
}
}
