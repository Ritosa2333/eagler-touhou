// EX关底中组弹幕时间轴的精确binary64常量与原编码零终止符卡名称。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou::ex_boss_middle_projectile_detail {
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6a7708:return 0x3f847ae147ae147bull;
case 0x6aca58:return 0xbff0000000000000ull;
case 0x6b25b8:return 0x4059000000000000ull;
case 0x6b25f8:return 0x403e000000000000ull;
case 0x6d1840:return 0xc040000000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d18c8:return 0x407e000000000000ull;
case 0x6d18e8:return 0x4020000000000000ull;
case 0x6d1900:return 0x3fc999999999999aull;
case 0x6d1910:return 0x4014000000000000ull;
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d1950:return 0x3f50624dd2f1a9fcull;
case 0x6d1968:return 0x3f9eb851eb851eb8ull;
case 0x6d19e8:return 0x4079400000000000ull;
case 0x6d19f0:return 0xc034000000000000ull;
case 0x6d19f8:return 0x407d400000000000ull;
case 0x6d1a38:return 0x407a000000000000ull;
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1ae8:return 0x4044000000000000ull;
case 0x6d1af0:return 0x406c000000000000ull;
case 0x6d1b10:return 0x3feeb851eb851eb8ull;
case 0x6d1b68:return 0x3f8a9fbe76c8b439ull;
case 0x6d1b70:return 0x3fb1eb851eb851ecull;
case 0x6d1b78:return 0x407b800000000000ull;
case 0x6d1b80:return 0x4077800000000000ull;
case 0x6d1ba8:return 0x402e000000000000ull;
case 0x6d1c60:return 0x40b88b0000000000ull;
default:throw std::out_of_range("EX middle constant");}}
}
