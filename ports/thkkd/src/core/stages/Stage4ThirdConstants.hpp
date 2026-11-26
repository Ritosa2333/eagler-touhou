// 原四面后半段时间轴与敌依赖使用的精确binary64常量。
#pragma once
#include <cstdint>
namespace kaikeidou { namespace stage4_third_detail {
inline constexpr std::uint64_t constant(std::uint32_t a){switch(a){
case 0x6d1b58:return 0xc03e000000000000ull;
case 0x6d1b60:return 0x3ff921fb54442d28ull;
case 0x6d1bb0:return 0x3fc0a3d70a3d70a4ull;
case 0x6d1880:return 0x3fe999999999999aull;
case 0x6d1e00:return 0x3faeb851eb851eb8ull;
case 0x6d1b70:return 0x3fb1eb851eb851ecull;
case 0x6d1900:return 0x3fc999999999999aull;
case 0x6d19c0:return 0x3fe0000000000000ull;
case 0x6d1bb8:return 0x4079e00000000000ull;
case 0x6b25f8:return 0x403e000000000000ull;
case 0x6d1ee8:return 0xbfe999999999999aull;
case 0x6d24f8:return 0x3fbc28f5c28f5c29ull;
case 0x6d1fa8:return 0x3fbeb851eb851eb8ull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d19f0:return 0xc034000000000000ull;
case 0x6d1910:return 0x4014000000000000ull;
case 0x6d1878:return 0x4050000000000000ull;
case 0x6d22c8:return 0x4076000000000000ull;
case 0x6d18d8:return 0x4074000000000000ull;
case 0x6d1de0:return 0xbff921fb54442d28ull;
case 0x6d1a08:return 0x3f947ae147ae147bull;
case 0x6d1a00:return 0x3ff3333333333333ull;
case 0x6d22d8:return 0x3fe921fb54442d28ull;
case 0x6b25b8:return 0x4059000000000000ull;
case 0x6d1b08:return 0x3fef0a3d70a3d70aull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d2688:return 0x3fd47ae147ae147bull;
case 0x6d1d68:return 0x407de00000000000ull;
case 0x6d1a88:return 0x4049000000000000ull;
case 0x6d1b28:return 0x3ff6666666666666ull;
case 0x6d1b90:return 0x4039000000000000ull;
case 0x6d1858:return 0x4066800000000000ull;
default:return 0;}}
}}
