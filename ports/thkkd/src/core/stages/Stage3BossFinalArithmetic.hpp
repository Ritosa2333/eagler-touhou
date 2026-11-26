#pragma once
#include "core/stages/Stage3BossFinalConstants.hpp"
#include "core/stages/Stage3BossTimelineContext.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/shared/FloorToInt32.hpp"
#include "core/math/FiniteDivide.hpp"
#include <stdexcept>
namespace kaikeidou {
inline std::uint32_t stage3_boss_final_floor(std::uint64_t value){return floor_to_int32(value);}
inline std::uint32_t stage3_boss_final_convert(std::uint64_t value){return binary_float::truncate(binary_float::binary64(value));}
inline std::uint64_t stage3_boss_final_degrees(std::uint64_t value,unsigned precision){using namespace binary_float;using stage3_boss_final_detail::constant;return to_binary64(divide(multiply(binary64(value),binary64(constant(0x6d1860)),precision),binary64(constant(0x6d1858)),precision));}
namespace stage3_boss_final_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:v;}
inline std::int64_t remainder(std::uint32_t a,std::uint32_t b){if(!b)throw std::domain_error("Stage3 final zero divisor");if(a==0x80000000u&&b==0xffffffffu)throw std::domain_error("Stage3 final quotient overflow");return signed_word(a)%signed_word(b);}
struct Arithmetic{
 unsigned bits;using N=binary_float::Number;
 N q(std::uint64_t v)const{return binary_float::binary64(v);}N c(std::uint32_t a)const{return q(constant(a));}N i(std::uint32_t v)const{return binary_float::integer(v);}
 N add(N a,N b)const{return binary_float::add(a,b,bits);}N sub(N a,N b)const{b.negative=!b.negative;return add(a,b);}N mul(N a,N b)const{return binary_float::multiply(a,b,bits);}N div(N a,N b)const{return binary_float::divide(a,b,bits);}std::uint64_t store(N a)const{return binary_float::to_binary64(a);}
};
inline int compare(binary_float::Number a,binary_float::Number b){
 if(!a.mantissa)a.negative=false;if(!b.mantissa)b.negative=false;
 if(a.negative!=b.negative)return a.negative?-1:1;
 int order=0;if(!a.mantissa||!b.mantissa)order=(a.mantissa!=0)-(b.mantissa!=0);else if(a.exponent!=b.exponent)order=a.exponent<b.exponent?-1:1;else if(a.mantissa!=b.mantissa)order=a.mantissa<b.mantissa?-1:1;
 return a.negative?-order:order;
}
inline std::uint64_t real(std::int64_t index,const PatternParameters& p,const BossStageContext& boss,const StagePatternContext& extra,const Stage3BossTimelineContext& final){
 if(index>=0&&index<2)return p.reals[static_cast<std::size_t>(index)];if(index>=2&&index<4)return boss.extra_reals[static_cast<std::size_t>(index-2)];if(index==4)return extra.real_4;
 if(index>=5&&index<10)return final.reals_5_to_9[static_cast<std::size_t>(index-5)];if(index>=10&&index<20)return extra.reals_10_to_19[static_cast<std::size_t>(index-10)];if(index>=20&&index<30)return final.reals_20_to_29[static_cast<std::size_t>(index-20)];throw std::out_of_range("Stage3 final real index outside mapped 0..29");
}
}
}
