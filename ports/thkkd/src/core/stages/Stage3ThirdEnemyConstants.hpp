// 原三面后半段敌53至54敌行为使用的精确binary64常量。
#pragma once
#include <cstdint>
namespace kaikeidou { namespace stage3_third_enemy_detail {
inline constexpr std::uint64_t constant(std::uint32_t a){switch(a){
case 0x6d1ce8:return 0x4002666666666666ull;
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d1ff8:return 0x3ffccccccccccccdull;
case 0x6d2488:return 0x3ff4cccccccccccdull;
case 0x6d1900:return 0x3fc999999999999aull;
case 0x6d18e0:return 0x3fd999999999999aull;
case 0x6d19c0:return 0x3fe0000000000000ull;
case 0x6d1c18:return 0x4010000000000000ull;
case 0x6d19f0:return 0xc034000000000000ull;
case 0x6d19e8:return 0x4079400000000000ull;
case 0x6d1de0:return 0xbff921fb54442d28ull;
case 0x6d1b60:return 0x3ff921fb54442d28ull;
case 0x6d2548:return 0x4003333333333333ull;
case 0x6d1e40:return 0x3ff999999999999aull;
case 0x6d1b28:return 0x3ff6666666666666ull;
case 0x6d2a70:return 0x3ff4000000000000ull;
case 0x6d2a68:return 0x3ffc000000000000ull;
case 0x6d1a00:return 0x3ff3333333333333ull;
case 0x6d1e20:return 0x4030000000000000ull;
case 0x6d1890:return 0x3ff2666666666666ull;
case 0x6d1d18:return 0x4048000000000000ull;
case 0x6d1f50:return 0x3ff199999999999aull;
case 0x6d1c58:return 0x4058000000000000ull;
case 0x6d2ae8:return 0x3ff0cccccccccccdull;
case 0x6d18c0:return 0x4064000000000000ull;
case 0x6d1d88:return 0x406e000000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d1858:return 0x4066800000000000ull;
default:return 0;}}
}}
