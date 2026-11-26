// EX关底中段末组弹幕时间轴的精确binary64常量与原编码零终止符卡名称。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou::ex_boss_middle_final_detail {
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6a7708:return 0x3f847ae147ae147bull;
case 0x6b25f8:return 0x403e000000000000ull;
case 0x6d1828:return 0x4038000000000000ull;
case 0x6d1840:return 0xc040000000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d1898:return 0x4024000000000000ull;
case 0x6d18c8:return 0x407e000000000000ull;
case 0x6d18e0:return 0x3fd999999999999aull;
case 0x6d1950:return 0x3f50624dd2f1a9fcull;
case 0x6d19c0:return 0x3fe0000000000000ull;
case 0x6d19d0:return 0x407a400000000000ull;
case 0x6d1a28:return 0x4078800000000000ull;
case 0x6d1a30:return 0xc020000000000000ull;
case 0x6d1a38:return 0x407a000000000000ull;
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1a68:return 0x407d000000000000ull;
case 0x6d1a78:return 0x4079000000000000ull;
case 0x6d1a90:return 0x4054000000000000ull;
case 0x6d1aa0:return 0x3f789374bc6a7efaull;
case 0x6d1ad0:return 0x3ff8000000000000ull;
case 0x6d1ae8:return 0x4044000000000000ull;
case 0x6d1af0:return 0x406c000000000000ull;
case 0x6d1af8:return 0x4072c00000000000ull;
case 0x6d1b00:return 0x3fe8000000000000ull;
case 0x6d1b08:return 0x3fef0a3d70a3d70aull;
case 0x6d1b28:return 0x3ff6666666666666ull;
case 0x6d1b30:return 0x3f6f212d77318fc5ull;
case 0x6d1b38:return 0x4056800000000000ull;
case 0x6d1b50:return 0x3eb0c6f7a0b5ed8dull;
case 0x6d1ba0:return 0x3fc921fb54442d28ull;
case 0x6d1ba8:return 0x402e000000000000ull;
case 0x6d1cc0:return 0x4062c00000000000ull;
case 0x6d1d00:return 0x405e000000000000ull;
case 0x6d1f50:return 0x3ff199999999999aull;
default:throw std::out_of_range("EX middle final constant");}}
}
