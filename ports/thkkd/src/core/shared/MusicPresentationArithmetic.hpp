// 原FILD/FDIV/FMUL及6968C0的binary64存回和截断语义。
#pragma once
#include "core/math/FiniteDivide.hpp"
#include "core/player/OptionsMotion.hpp"
namespace kaikeidou::music_presentation_detail {
inline std::int64_t signed_word(std::uint32_t n){return n>>31?std::int64_t(n)-0x100000000ll:std::int64_t(n);}
inline std::uint32_t truncate_sse(binary_float::Number n){
 n=binary_float::binary64(binary_float::to_binary64(n));
 return n.exponent>=31?0x80000000u:binary_float::truncate(n);
}
inline std::uint32_t volume(std::uint32_t value,unsigned precision){
 using namespace binary_float;
 return truncate_sse(multiply(divide(integer(value),integer(100),precision),integer(255),precision));
}
inline std::uint32_t filename_bank(std::uint32_t track,unsigned precision){
 using namespace binary_float;
 const auto n=binary64(to_binary64(divide(integer(track+1u),integer(2),precision)));
 auto result=truncate(n);
 if(n.negative&&n.mantissa&&(n.exponent<0||(n.exponent<63&&(n.mantissa&((std::uint64_t(1)<<(63-n.exponent))-1u)))))--result;
 return result;
}
}
