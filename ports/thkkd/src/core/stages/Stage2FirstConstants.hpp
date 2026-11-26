// 二面首段原binary64常量。
#pragma once
#include <cstdint>
namespace kaikeidou::stage2_first_detail {
inline constexpr std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6d1858:return 0x4066800000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d19f0:return 0xc034000000000000ull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1ad8:return 0x4008000000000000ull;
case 0x6d1b18:return 0x4004000000000000ull;
case 0x6d1d78:return 0x4060000000000000ull;
case 0x6d22c0:return 0xc024000000000000ull;
case 0x6d2758:return 0xc02e000000000000ull;
case 0x6d2760:return 0xbff4000000000000ull;
case 0x6d2768:return 0x3fe3d70a3d70a3d7ull;
case 0x6d2770:return 0x4033333333333333ull;
case 0x6d2778:return 0x4043333333333333ull;
default:return 0;}}
}
