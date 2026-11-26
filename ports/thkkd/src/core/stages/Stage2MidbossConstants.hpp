// 原二面中Boss依赖使用的精确binary64常量。
#pragma once
#include <cstdint>
namespace kaikeidou { namespace stage2_midboss_detail {
inline constexpr std::uint64_t constant(std::uint32_t a){switch(a){
case 0x6d1a88:return 0x4049000000000000ull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d2b30:return 0x3f916872b020c49cull;
case 0x6d1898:return 0x4024000000000000ull;
case 0x6d18c0:return 0x4064000000000000ull;
case 0x6d2750:return 0x3ff0c152382d7370ull;
case 0x6d1fa8:return 0x3fbeb851eb851eb8ull;
case 0x6d1b18:return 0x4004000000000000ull;
case 0x6d1ce8:return 0x4002666666666666ull;
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d1c80:return 0x3ffb333333333333ull;
case 0x6d1920:return 0x3fd3333333333333ull;
case 0x6d1d10:return 0x4078900000000000ull;
case 0x6d1d08:return 0x407c900000000000ull;
case 0x6d1c90:return 0x400c000000000000ull;
case 0x6d1b08:return 0x3fef0a3d70a3d70aull;
case 0x6a7888:return 0x3f1a36e2eb1c432dull;
case 0x6d1840:return 0xc040000000000000ull;
case 0x6d1a38:return 0x407a000000000000ull;
case 0x6d18c8:return 0x407e000000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d1858:return 0x4066800000000000ull;
default:return 0;}}
}}
