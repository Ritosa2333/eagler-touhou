// 六面关底前组逐指令精度算术与有符号整数除法。
#pragma once
#include "core/stages/Stage6BossEarlyConstants.hpp"
#include "core/math/FiniteDivide.hpp"
#include "core/math/FiniteSqrt.hpp"
#include "core/stages/Stage6BossEarlyRecords.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/entities/EnemyPool.hpp"
namespace kaikeidou {
inline std::uint32_t stage6_boss_early_pick_integer(const std::uint32_t& d,const std::array<std::uint32_t,6>& v){return v[d<6?d:3];}
inline std::uint64_t stage6_boss_early_pick_real(const std::uint32_t& d,const std::array<std::uint64_t,6>& v){return v[d<6?d:3];}
template<class Record> inline void stage6_boss_early_ease(Record& e,std::size_t offset,std::uint64_t target,std::uint64_t divisor,unsigned bits){e.quad(offset,ease_option_value(e.quad(offset),target,divisor,bits));}
inline std::uint64_t stage6_boss_early_absolute(std::uint64_t value){return value&0x7fffffffffffffffull;}
inline binary_float::Number stage6_boss_early_sqrt(std::uint64_t value,unsigned precision){return finite_square_root(value,precision);}
inline void stage6_boss_early_recolor(ProjectilePools& pool,std::uint32_t index,std::uint32_t r,std::uint32_t g,std::uint32_t b){stage6_boss_early_detail::PoolRecord record{pool,0xa8e660u+index*240u};record.word(0x18,r);record.word(0x1c,g);record.word(0x20,b);}
inline std::int32_t stage6_boss_early_remove100(ProjectilePools& pool,std::uint32_t index){stage6_boss_early_detail::PoolRecord record{pool,0xa8e660u+index*240u};if(record.word(0)==1||record.word(0)==2){record.word(0,3);record.word(0x10,0);record.word(8,0);if(record.word(0x2c)==0)record.word(0x2c,1);}return static_cast<std::int32_t>(index*240u);}
namespace stage6_boss_early_detail {
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
