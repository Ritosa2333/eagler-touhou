// 原关底时间轴读取的binary64常量。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou::stage4_first_detail {
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6a7708:return 0x3f847ae147ae147bull;
case 0x6a7eb8:return 0x406fe00000000000ull;
case 0x6b25b8:return 0x4059000000000000ull;
case 0x6b25f8:return 0x403e000000000000ull;
case 0x6b2650:return 0xbfe0000000000000ull;
case 0x6d1810:return 0x3fa47ae147ae147bull;
case 0x6d18f0:return 0x3feccccccccccccdull;
case 0x6d1900:return 0x3fc999999999999aull;
case 0x6d1910:return 0x4014000000000000ull;
case 0x6d1920:return 0x3fd3333333333333ull;
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d1960:return 0x3fb999999999999aull;
case 0x6d19c0:return 0x3fe0000000000000ull;
case 0x6d19c8:return 0x3fcd70a3d70a3d71ull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1a88:return 0x4049000000000000ull;
case 0x6d1a90:return 0x4054000000000000ull;
case 0x6d1ad0:return 0x3ff8000000000000ull;
case 0x6d1ae8:return 0x4044000000000000ull;
case 0x6d1b28:return 0x3ff6666666666666ull;
case 0x6d1b58:return 0xc03e000000000000ull;
case 0x6d1bb0:return 0x3fc0a3d70a3d70a4ull;
case 0x6d1bb8:return 0x4079e00000000000ull;
case 0x6d1cc0:return 0x4062c00000000000ull;
case 0x6d1d78:return 0x4060000000000000ull;
case 0x6d1dd0:return 0xc064000000000000ull;
case 0x6d1e48:return 0x4040000000000000ull;
case 0x6d1fc0:return 0xbff4cccccccccccdull;
case 0x6d1ff8:return 0x3ffccccccccccccdull;
case 0x6d2108:return 0x4061c00000000000ull;
case 0x6d2130:return 0x4000cccccccccccdull;
case 0x6d2138:return 0x4000c152382d7370ull;
case 0x6d2288:return 0x4076200000000000ull;
case 0x6d23e0:return 0x4075800000000000ull;
case 0x6d2488:return 0x3ff4cccccccccccdull;
case 0x6d2548:return 0x4003333333333333ull;
case 0x6d25a8:return 0x400599999999999aull;
case 0x6d2628:return 0x3fc851eb851eb852ull;
case 0x6d2630:return 0x3fcae147ae147ae1ull;
case 0x6d2638:return 0x3fcc28f5c28f5c29ull;
case 0x6d2640:return 0xbff8000000000000ull;
case 0x6d2648:return 0x4064400000000000ull;
case 0x6d2650:return 0x406bc00000000000ull;
case 0x6d2658:return 0x4022000000000000ull;
case 0x6d2660:return 0x400199999999999aull;
case 0x6d2668:return 0xbfb1eb851eb851ecull;
default:throw std::out_of_range("Stage4 first constant");}}
}
