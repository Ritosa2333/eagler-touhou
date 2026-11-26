// 原六面关底后组敌弹使用的精确binary64常量。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou { namespace stage6_boss_final_detail {
inline constexpr std::uint64_t constant(std::uint32_t a){switch(a){
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d1e20:return 0x4030000000000000ull;
case 0x6d1e48:return 0x4040000000000000ull;
case 0x6d1b58:return 0xc03e000000000000ull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d18c0:return 0x4064000000000000ull;
case 0x6d2208:return 0x401921fb54442d28ull;
case 0x6d25f0:return 0x4004cccccccccccdull;
case 0x6d1c00:return 0x407e800000000000ull;
case 0x6b25f8:return 0x403e000000000000ull;
case 0x6d3088:return 0x3fbc6a7ef9db22d1ull;
case 0x6d1bb8:return 0x4079e00000000000ull;
case 0x6d18e8:return 0x4020000000000000ull;
case 0x6d1960:return 0x3fb999999999999aull;
case 0x6d18c8:return 0x407e000000000000ull;
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6a7708:return 0x3f847ae147ae147bull;
case 0x6d1a88:return 0x4049000000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d1858:return 0x4066800000000000ull;
default:return 0;}}
}}
