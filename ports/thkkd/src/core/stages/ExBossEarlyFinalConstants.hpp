// EX关底前段末组弹幕时间轴的精确binary64常量与原编码零终止符卡名称。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou::ex_boss_early_final_detail {
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6a7708:return 0x3f847ae147ae147bull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d1898:return 0x4024000000000000ull;
case 0x6d1900:return 0x3fc999999999999aull;
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d1950:return 0x3f50624dd2f1a9fcull;
case 0x6d1960:return 0x3fb999999999999aull;
case 0x6d19f0:return 0xc034000000000000ull;
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6d1a78:return 0x4079000000000000ull;
case 0x6d1a88:return 0x4049000000000000ull;
case 0x6d1a90:return 0x4054000000000000ull;
case 0x6d1aa0:return 0x3f789374bc6a7efaull;
case 0x6d1ab0:return 0x4069000000000000ull;
case 0x6d1ad8:return 0x4008000000000000ull;
case 0x6d1ae8:return 0x4044000000000000ull;
case 0x6d1af0:return 0x406c000000000000ull;
case 0x6d1b40:return 0x3ee4f8b588e368f1ull;
case 0x6d1b48:return 0x3f8eb851eb851eb8ull;
case 0x6d1b58:return 0xc03e000000000000ull;
case 0x6d1b60:return 0x3ff921fb54442d28ull;
case 0x6d1c60:return 0x40b88b0000000000ull;
case 0x6d1de0:return 0xbff921fb54442d28ull;
case 0x6d1f68:return 0x4013333333333333ull;
case 0x6d1f98:return 0x407ac00000000000ull;
case 0x6d1ff0:return 0xc059000000000000ull;
case 0x6d2000:return 0x407e400000000000ull;
case 0x6d2410:return 0x401c000000000000ull;
case 0x6d2480:return 0x400b333333333333ull;
case 0x6d2548:return 0x4003333333333333ull;
case 0x6d26e0:return 0x400ccccccccccccdull;
case 0x6d26e8:return 0x4010666666666666ull;
case 0x6d2830:return 0x3f53a92a30553261ull;
case 0x6d2838:return 0x407da00000000000ull;
case 0x6d2840:return 0xc056800000000000ull;
case 0x6d2848:return 0x3e9421f5f40d8376ull;
case 0x6d2850:return 0x4109d48000000000ull;
case 0x6d2858:return 0x4077c00000000000ull;
case 0x6d2860:return 0x3f9758e219652bd4ull;
case 0x6d2868:return 0x3fad97f62b6ae7d5ull;
case 0x6d2be8:return 0x4018cccccccccccdull;
case 0x6d2cb8:return 0x4016000000000000ull;
case 0x6d3198:return 0x4081200000000000ull;
case 0x6d31a0:return 0x4010cccccccccccdull;
default:throw std::out_of_range("EX early final constant");}}
}
