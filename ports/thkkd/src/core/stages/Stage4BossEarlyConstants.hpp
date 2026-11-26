// 原四面关底早期敌弹使用的精确binary64常量。
#pragma once
#include <cstdint>
namespace kaikeidou { namespace stage4_boss_early_detail {
inline constexpr std::uint64_t constant(std::uint32_t a){switch(a){
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1e00:return 0x3faeb851eb851eb8ull;
case 0x6d1940:return 0x3fef5c28f5c28f5cull;
case 0x6d1a88:return 0x4049000000000000ull;
case 0x6d1b38:return 0x4056800000000000ull;
case 0x6d1920:return 0x3fd3333333333333ull;
case 0x6b25f8:return 0x403e000000000000ull;
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6d1900:return 0x3fc999999999999aull;
case 0x6b25b8:return 0x4059000000000000ull;
case 0x6a7708:return 0x3f847ae147ae147bull;
case 0x6d1aa0:return 0x3f789374bc6a7efaull;
case 0x6d18c0:return 0x4064000000000000ull;
case 0x6d1960:return 0x3fb999999999999aull;
case 0x6d1a78:return 0x4079000000000000ull;
case 0x6d1a00:return 0x3ff3333333333333ull;
case 0x6d1e70:return 0xbff3333333333333ull;
case 0x6d1aa8:return 0x404e000000000000ull;
case 0x6d2188:return 0x40af400000000000ull;
case 0x6d1a30:return 0xc020000000000000ull;
case 0x6d1ab0:return 0x4069000000000000ull;
case 0x6d1950:return 0x3f50624dd2f1a9fcull;
case 0x6d22a0:return 0xbfb999999999999aull;
case 0x6d1af0:return 0x406c000000000000ull;
case 0x6d1c70:return 0x4074a00000000000ull;
case 0x6d1a20:return 0x3fee666666666666ull;
case 0x6aca58:return 0xbff0000000000000ull;
case 0x6d1868:return 0x3ff0000000000000ull;
case 0x6d19e8:return 0x4079400000000000ull;
case 0x6d2588:return 0x4046000000000000ull;
case 0x6d1b60:return 0x3ff921fb54442d28ull;
case 0x6d18e0:return 0x3fd999999999999aull;
case 0x6d1c68:return 0x4009eec82110f9f5ull;
case 0x6d1c60:return 0x40b88b0000000000ull;
case 0x6d1a48:return 0x407c800000000000ull;
case 0x6d1a28:return 0x4078800000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d1858:return 0x4066800000000000ull;
default:return 0;}}
}}
