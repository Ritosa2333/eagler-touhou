// EX中Boss原binary64常量及符卡名称原字节。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou { namespace ex_midboss_detail {
inline constexpr std::uint64_t constant(std::uint32_t a){switch(a){
case 0x6a7708:return 0x3f847ae147ae147bull;
case 0x6a7888:return 0x3f1a36e2eb1c432dull;
case 0x6b25b8:return 0x4059000000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d18f8:return 0x3fed70a3d70a3d71ull;
case 0x6d1900:return 0x3fc999999999999aull;
case 0x6d1940:return 0x3fef5c28f5c28f5cull;
case 0x6d1950:return 0x3f50624dd2f1a9fcull;
case 0x6d1960:return 0x3fb999999999999aull;
case 0x6d19c0:return 0x3fe0000000000000ull;
case 0x6d19f0:return 0xc034000000000000ull;
case 0x6d1a20:return 0x3fee666666666666ull;
case 0x6d1a28:return 0x4078800000000000ull;
case 0x6d1a30:return 0xc020000000000000ull;
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6d1a48:return 0x407c800000000000ull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1aa8:return 0x404e000000000000ull;
case 0x6d1ae8:return 0x4044000000000000ull;
case 0x6d1b18:return 0x4004000000000000ull;
case 0x6d1b38:return 0x4056800000000000ull;
case 0x6d1b58:return 0xc03e000000000000ull;
case 0x6d1b60:return 0x3ff921fb54442d28ull;
case 0x6d1c58:return 0x4058000000000000ull;
case 0x6d1c60:return 0x40b88b0000000000ull;
case 0x6d1cc0:return 0x4062c00000000000ull;
case 0x6d1ce8:return 0x4002666666666666ull;
case 0x6d1d00:return 0x405e000000000000ull;
case 0x6d1d50:return 0x4051800000000000ull;
case 0x6d1e00:return 0x3faeb851eb851eb8ull;
case 0x6d1f08:return 0x4061800000000000ull;
case 0x6d1f20:return 0x4092c00000000000ull;
case 0x6d1fa0:return 0x3fa1eb851eb851ecull;
case 0x6d1fa8:return 0x3fbeb851eb851eb8ull;
case 0x6d2028:return 0x407f400000000000ull;
case 0x6d2160:return 0x4076c00000000000ull;
case 0x6d2188:return 0x40af400000000000ull;
case 0x6d21f0:return 0x4060400000000000ull;
case 0x6d2660:return 0x400199999999999aull;
case 0x6d2680:return 0xc041800000000000ull;
case 0x6d2780:return 0x405b800000000000ull;
case 0x6d2870:return 0x4095e00000000000ull;
case 0x6d2878:return 0x3f90624dd2f1a9fcull;
case 0x6d2aa0:return 0x4074800000000000ull;
case 0x6d2fa0:return 0x406fc00000000000ull;
case 0x6d2fb0:return 0x3fa0e5604189374cull;
case 0x6d2fb8:return 0x3fc810624dd2f1aaull;
default:throw std::out_of_range("EX midboss constant");}}
inline const char* spell_name(std::uint32_t a){switch(a){
case 0x6c5f8c:return "\xc2\xfa\xb3\xb1\xa1\xb8\xd1\xce\xd3\xaf\xd6\xe9\xa1\xb9";
case 0x6c5f9c:return "\xc2\xe4\xb3\xb1\xa1\xb8\xd1\xce\xb8\xc9\xd6\xe9\xa1\xb9";
case 0x6c5fac:return "\xc9\xf1\xc3\xd8\xa1\xb8\xb3\xf6\xd1\xce\xb5\xc4\xc9\xf1\xc3\xd8\xca\xaf\xbe\xca\xa1\xb9";
default:throw std::out_of_range("EX midboss spell name");}}
}}
