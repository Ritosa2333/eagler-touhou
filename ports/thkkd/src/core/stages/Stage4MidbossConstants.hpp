// 原四面中Boss时间轴与敌弹使用的精确binary64常量。
#pragma once
#include <cstdint>
namespace kaikeidou { namespace stage4_midboss_detail {
inline constexpr std::uint64_t constant(std::uint32_t a){switch(a){
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1c00:return 0x407e800000000000ull;
case 0x6d1a90:return 0x4054000000000000ull;
case 0x6b25b8:return 0x4059000000000000ull;
case 0x6d18e0:return 0x3fd999999999999aull;
case 0x6d2620:return 0x401a000000000000ull;
case 0x6d1a88:return 0x4049000000000000ull;
case 0x6d1aa8:return 0x404e000000000000ull;
case 0x6d1920:return 0x3fd3333333333333ull;
case 0x6b25f8:return 0x403e000000000000ull;
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6d1900:return 0x3fc999999999999aull;
case 0x6a7708:return 0x3f847ae147ae147bull;
case 0x6d1aa0:return 0x3f789374bc6a7efaull;
case 0x6d22a8:return 0x40ae000000000000ull;
case 0x6d1960:return 0x3fb999999999999aull;
case 0x6d1a00:return 0x3ff3333333333333ull;
case 0x6d1e70:return 0xbff3333333333333ull;
case 0x6d1a68:return 0x407d000000000000ull;
case 0x6d1a80:return 0xc030000000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d1858:return 0x4066800000000000ull;
default:return 0;}}
}}
