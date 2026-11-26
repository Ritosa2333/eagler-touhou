// 原三面关底早段依赖使用的精确binary64常量。
#pragma once
#include <cstdint>
namespace kaikeidou { namespace stage3_boss_early_detail {
inline constexpr std::uint64_t constant(std::uint32_t a){switch(a){
case 0x6d1b18:return 0x4004000000000000ull;
case 0x6d1b60:return 0x3ff921fb54442d28ull;
case 0x6d1d00:return 0x405e000000000000ull;
case 0x6d22b0:return 0x4060800000000000ull;
case 0x6d1ae8:return 0x4044000000000000ull;
case 0x6d1d50:return 0x4051800000000000ull;
case 0x6d1e38:return 0x4089000000000000ull;
case 0x6d1950:return 0x3f50624dd2f1a9fcull;
case 0x6d18e0:return 0x3fd999999999999aull;
case 0x6d1a80:return 0xc030000000000000ull;
case 0x6d22d0:return 0x4005eec82110f9f5ull;
case 0x6d1a78:return 0x4079000000000000ull;
case 0x6d21f0:return 0x4060400000000000ull;
case 0x6d1e48:return 0x4040000000000000ull;
case 0x6d22c8:return 0x4076000000000000ull;
case 0x6d22c0:return 0xc024000000000000ull;
case 0x6d22b8:return 0x3f4a36e2eb1c432dull;
case 0x6d1e00:return 0x3faeb851eb851eb8ull;
case 0x6d1ee8:return 0xbfe999999999999aull;
case 0x6d1bd0:return 0x3fefae147ae147aeull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d31f0:return 0x4086000000000000ull;
case 0x6d2670:return 0xc070000000000000ull;
case 0x6d1ec0:return 0x4084000000000000ull;
case 0x6d2460:return 0xc060000000000000ull;
case 0x6d1998:return 0x4080000000000000ull;
case 0x6d1b08:return 0x3fef0a3d70a3d70aull;
case 0x6d18c8:return 0x407e000000000000ull;
case 0x6d1858:return 0x4066800000000000ull;
default:return 0;}}
}}
