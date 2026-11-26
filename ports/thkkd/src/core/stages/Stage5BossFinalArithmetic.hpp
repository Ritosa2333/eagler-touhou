// 五面后半段逐指令精度算术与有符号整数除法。
#pragma once
#include "core/stages/Stage5BossFinalConstants.hpp"
#include "core/math/FiniteDivide.hpp"
#include "core/stages/Stage3InitialMath.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/entities/EnemyPool.hpp"
namespace kaikeidou {
inline std::uint32_t stage5_boss_final_pick_integer(const std::uint32_t& d,const std::array<std::uint32_t,6>& v){return v[d<6?d:3];}
inline std::uint64_t stage5_boss_final_pick_real(const std::uint32_t& d,const std::array<std::uint64_t,6>& v){return v[d<6?d:3];}
template<class Record> inline void stage5_boss_final_ease(Record& e,std::size_t offset,std::uint64_t target,std::uint64_t divisor,unsigned bits){e.quad(offset,ease_option_value(e.quad(offset),target,divisor,bits));}
inline std::uint32_t stage5_boss_final_color_integer(binary_float::Number value,bool sse){
 if(sse)value=binary_float::rounded(value,0,53);
 if(!value.mantissa||value.exponent<0)return 0;
 if(value.exponent>=63)return sse?0x80000000u:0u;
 const auto magnitude=value.mantissa>>(63-value.exponent);
 if(sse&&(magnitude>(value.negative?0x80000000ull:0x7fffffffull)))return 0x80000000u;
 const auto low=static_cast<std::uint32_t>(magnitude);return value.negative?0u-low:low;
}
namespace stage5_boss_final_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:std::int64_t(v);}
inline std::int64_t remainder(std::uint32_t a,std::uint32_t b){if(!b)throw std::domain_error("Original stage5 IDIV zero divisor");if(a==0x80000000u&&b==0xffffffffu)throw std::domain_error("Original stage5 IDIV overflow");return signed_word(a)%signed_word(b);}
inline int compare(binary_float::Number a,binary_float::Number b){
 if(!a.mantissa&&!b.mantissa)return 0;if(!a.mantissa)return b.negative?1:-1;if(!b.mantissa)return a.negative?-1:1;
 if(a.negative!=b.negative)return a.negative?-1:1;const int magnitude=a.exponent!=b.exponent?(a.exponent<b.exponent?-1:1):a.mantissa==b.mantissa?0:a.mantissa<b.mantissa?-1:1;return a.negative?-magnitude:magnitude;
}
struct Arithmetic {using Number=binary_float::Number;unsigned precision;
 Number q(std::uint64_t v)const{return binary_float::binary64(v);}Number c(std::uint32_t a)const{return q(constant(a));}Number i(std::uint32_t v)const{return binary_float::integer(v);}
 Number add(Number a,Number b)const{return binary_float::add(a,b,precision);}Number sub(Number a,Number b)const{b.negative=!b.negative;return add(a,b);}Number mul(Number a,Number b)const{return binary_float::multiply(a,b,precision);}Number div(Number a,Number b)const{return binary_float::divide(a,b,precision);}std::uint64_t store(Number n)const{return binary_float::to_binary64(n);}
};
}
}
