// 六面首段时间轴的精确binary64常量与原编码零终止符卡名称。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou::stage6_first_detail {
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6b25b8:return 0x4059000000000000ull;
case 0x6d1868:return 0x3ff0000000000000ull;
case 0x6d1898:return 0x4024000000000000ull;
case 0x6d18b0:return 0x3fe3333333333333ull;
case 0x6d18f0:return 0x3feccccccccccccdull;
case 0x6d1900:return 0x3fc999999999999aull;
case 0x6d1910:return 0x4014000000000000ull;
case 0x6d1920:return 0x3fd3333333333333ull;
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d1950:return 0x3f50624dd2f1a9fcull;
case 0x6d1960:return 0x3fb999999999999aull;
case 0x6d19e8:return 0x4079400000000000ull;
case 0x6d19f0:return 0xc034000000000000ull;
case 0x6d1a00:return 0x3ff3333333333333ull;
case 0x6d1a08:return 0x3f947ae147ae147bull;
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6d1a50:return 0x4079800000000000ull;
case 0x6d1a58:return 0xc038000000000000ull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1aa8:return 0x404e000000000000ull;
case 0x6d1ad0:return 0x3ff8000000000000ull;
case 0x6d1ad8:return 0x4008000000000000ull;
case 0x6d1af0:return 0x406c000000000000ull;
case 0x6d1b10:return 0x3feeb851eb851eb8ull;
case 0x6d1b18:return 0x4004000000000000ull;
case 0x6d1ba8:return 0x402e000000000000ull;
case 0x6d1bd8:return 0x407d800000000000ull;
case 0x6d1c60:return 0x40b88b0000000000ull;
case 0x6d1c90:return 0x400c000000000000ull;
case 0x6d1ce0:return 0x4006666666666666ull;
case 0x6d1cf0:return 0x3fd6666666666666ull;
case 0x6d1d18:return 0x4048000000000000ull;
case 0x6d1de8:return 0x405c000000000000ull;
case 0x6d1e40:return 0x3ff999999999999aull;
case 0x6d1e50:return 0x408f400000000000ull;
case 0x6d1f78:return 0x3fe199999999999aull;
case 0x6d1f98:return 0x407ac00000000000ull;
case 0x6d1fb8:return 0x3fd0000000000000ull;
case 0x6d2028:return 0x407f400000000000ull;
case 0x6d2168:return 0x407c200000000000ull;
case 0x6d2208:return 0x401921fb54442d28ull;
case 0x6d2220:return 0xc004cccccccccccdull;
case 0x6d2228:return 0xbffccccccccccccdull;
case 0x6d2480:return 0x400b333333333333ull;
case 0x6d2488:return 0x3ff4cccccccccccdull;
case 0x6d24b0:return 0x40d1940000000000ull;
case 0x6d24b8:return 0x409f400000000000ull;
case 0x6d24c0:return 0x4008cccccccccccdull;
case 0x6d24c8:return 0x3f32599ed7c6fbd2ull;
case 0x6d24d0:return 0x40e2c00000000000ull;
case 0x6d24d8:return 0xc016000000000000ull;
case 0x6d24e0:return 0x3fdccccccccccccdull;
case 0x6d24e8:return 0x3ff7333333333333ull;
case 0x6d29d0:return 0xbffb8861baaa938eull;
case 0x6d29d8:return 0x3ffb8861baaa938eull;
case 0x6d29e0:return 0xbff6bb94edddc6c2ull;
case 0x6d29e8:return 0x3ff6bb94edddc6c2ull;
case 0x6d29f0:return 0x40043a251e4793bdull;
case 0x6d29f8:return 0x4004ef5e41d4b6a6ull;
case 0x6d2a00:return 0x400636106d60809eull;
case 0x6d2a08:return 0x40083b8977628d13ull;
default:throw std::out_of_range("Stage6 first constant");}}
}
