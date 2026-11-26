// 自然调用域的FSINCOS结果：小角舍入界与完整float32网格对照见本批证据。
#pragma once
#include "core/math/BinaryFloat32.hpp"
#include "core/math/CrtSse2Trig.hpp"
#include <array>
namespace kaikeidou::stage6_float3_rotation_detail {
inline constexpr std::uint64_t maximum_angle64=0x3f8cac083126e979ull;
inline constexpr std::uint32_t maximum_angle32=0x3c656042u;
inline std::array<std::uint32_t,2> sincos32(std::uint64_t angle){
 using namespace binary_float;if((angle&0x7fffffffffffffffull)>maximum_angle64)throw std::out_of_range("Stage6 float3 angle outside verified finite domain");
 const auto input=to_binary32(binary64(angle));
 if((input&0x7fffffffu)<0x39800000u)return {{input,0x3f800000u}};
 const auto exact=to_binary64(finite_binary32(input));
 const auto cosine=to_binary32(binary64(crt_sse2::cosine_bits(exact)));
 const auto sine=to_binary32(binary64(crt_sse2::sine_bits(exact)));
 return {{sine,cosine}};
}
}
