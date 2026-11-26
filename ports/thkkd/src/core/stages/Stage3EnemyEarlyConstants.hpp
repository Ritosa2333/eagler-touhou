// 原三面首段敌43至46敌行为使用的精确binary64常量。
#pragma once
#include <cstdint>
namespace kaikeidou { namespace stage3_enemy_early_detail {
inline constexpr std::uint64_t constant(std::uint32_t a){switch(a){
case 0x6d1c20:return 0x4012000000000000ull;
case 0x6d18f0:return 0x3feccccccccccccdull;
case 0x6d1b60:return 0x3ff921fb54442d28ull;
case 0x6d1de0:return 0xbff921fb54442d28ull;
case 0x6d1920:return 0x3fd3333333333333ull;
case 0x6d1b28:return 0x3ff6666666666666ull;
case 0x6d1a00:return 0x3ff3333333333333ull;
case 0x6d1cf0:return 0x3fd6666666666666ull;
case 0x6d1f50:return 0x3ff199999999999aull;
case 0x6d1fb8:return 0x3fd0000000000000ull;
case 0x6d1c80:return 0x3ffb333333333333ull;
case 0x6d19f0:return 0xc034000000000000ull;
case 0x6b25b8:return 0x4059000000000000ull;
case 0x6d2530:return 0x3fa16872b020c49cull;
case 0x6d1a18:return 0x3fb47ae147ae147bull;
case 0x6d2b08:return 0x3fa78d4fdf3b645aull;
case 0x6d1b18:return 0x4004000000000000ull;
case 0x6d1928:return 0x3f9999999999999aull;
case 0x6d2b00:return 0x3ff5eec82110f9f5ull;
case 0x6d2af8:return 0xbff5eec82110f9f5ull;
case 0x6d1e40:return 0x3ff999999999999aull;
case 0x6d1ad0:return 0x3ff8000000000000ull;
case 0x6d2488:return 0x3ff4cccccccccccdull;
case 0x6d2660:return 0x400199999999999aull;
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d1ff8:return 0x3ffccccccccccccdull;
case 0x6d1ec0:return 0x4084000000000000ull;
case 0x6d1a20:return 0x3fee666666666666ull;
case 0x6d1868:return 0x3ff0000000000000ull;
case 0x6d1b58:return 0xc03e000000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d1858:return 0x4066800000000000ull;
default:return 0;}}
}}
