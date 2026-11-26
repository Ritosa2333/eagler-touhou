// 原关底时间轴读取的binary64常量。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou::stage3_first_detail {
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6aca58:return 0xbff0000000000000ull;
case 0x6b25b8:return 0x4059000000000000ull;
case 0x6d1920:return 0x3fd3333333333333ull;
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d1960:return 0x3fb999999999999aull;
case 0x6d19c0:return 0x3fe0000000000000ull;
case 0x6d19c8:return 0x3fcd70a3d70a3d71ull;
case 0x6d19f0:return 0xc034000000000000ull;
case 0x6d19f8:return 0x407d400000000000ull;
case 0x6d1a00:return 0x3ff3333333333333ull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1ad0:return 0x3ff8000000000000ull;
case 0x6d1b18:return 0x4004000000000000ull;
case 0x6d1b58:return 0xc03e000000000000ull;
case 0x6d1b88:return 0x4028000000000000ull;
case 0x6d1cb0:return 0x4078000000000000ull;
case 0x6d1ce0:return 0x4006666666666666ull;
case 0x6d1d78:return 0x4060000000000000ull;
case 0x6d1de0:return 0xbff921fb54442d28ull;
case 0x6d1e40:return 0x3ff999999999999aull;
case 0x6d1e48:return 0x4040000000000000ull;
case 0x6d1ee8:return 0xbfe999999999999aull;
case 0x6d2208:return 0x401921fb54442d28ull;
case 0x6d22c8:return 0x4076000000000000ull;
case 0x6d23c8:return 0x406d000000000000ull;
case 0x6d2410:return 0x401c000000000000ull;
case 0x6d2488:return 0x3ff4cccccccccccdull;
case 0x6d24f8:return 0x3fbc28f5c28f5c29ull;
case 0x6d2548:return 0x4003333333333333ull;
case 0x6d2658:return 0x4022000000000000ull;
case 0x6d2670:return 0xc070000000000000ull;
case 0x6d2680:return 0xc041800000000000ull;
case 0x6d2688:return 0x3fd47ae147ae147bull;
case 0x6d2690:return 0x3fd26e978d4fdf3bull;
case 0x6d2698:return 0x3fd0a3d70a3d70a4ull;
case 0x6d26a0:return 0xc022000000000000ull;
default:throw std::out_of_range("Stage3 first constant");}}
}
