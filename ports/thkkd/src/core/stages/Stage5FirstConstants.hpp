// 五面首段时间轴的精确binary64常量与原编码零终止符卡名称。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou::stage5_first_detail {
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6b25f8:return 0x403e000000000000ull;
case 0x6d1848:return 0xc044000000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d1880:return 0x3fe999999999999aull;
case 0x6d18e0:return 0x3fd999999999999aull;
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d1988:return 0x3fee147ae147ae14ull;
case 0x6d19c0:return 0x3fe0000000000000ull;
case 0x6d19e0:return 0x3f70624dd2f1a9fcull;
case 0x6d19e8:return 0x4079400000000000ull;
case 0x6d19f0:return 0xc034000000000000ull;
case 0x6d1a00:return 0x3ff3333333333333ull;
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1a68:return 0x407d000000000000ull;
case 0x6d1aa0:return 0x3f789374bc6a7efaull;
case 0x6d1aa8:return 0x404e000000000000ull;
case 0x6d1ae8:return 0x4044000000000000ull;
case 0x6d1af0:return 0x406c000000000000ull;
case 0x6d1b28:return 0x3ff6666666666666ull;
case 0x6d1b60:return 0x3ff921fb54442d28ull;
case 0x6d1bf0:return 0x4080800000000000ull;
case 0x6d1bf8:return 0xc054000000000000ull;
case 0x6d1c00:return 0x407e800000000000ull;
case 0x6d1c08:return 0x407a800000000000ull;
case 0x6d1c10:return 0x3fb70a3d70a3d70aull;
case 0x6d1c18:return 0x4010000000000000ull;
case 0x6d1c90:return 0x400c000000000000ull;
case 0x6d1cc0:return 0x4062c00000000000ull;
case 0x6d1cf0:return 0x3fd6666666666666ull;
case 0x6d1cf8:return 0x3f689374bc6a7efaull;
case 0x6d1d50:return 0x4051800000000000ull;
case 0x6d1dd8:return 0x3fe41b2f769cf0edull;
case 0x6d1de0:return 0xbff921fb54442d28ull;
case 0x6d1e48:return 0x4040000000000000ull;
case 0x6d1e70:return 0xbff3333333333333ull;
case 0x6d1f50:return 0x3ff199999999999aull;
case 0x6d1fa0:return 0x3fa1eb851eb851ecull;
case 0x6d2138:return 0x4000c152382d7370ull;
case 0x6d2298:return 0x3ff0147ae147ae14ull;
case 0x6d23e0:return 0x4075800000000000ull;
case 0x6d2488:return 0x3ff4cccccccccccdull;
case 0x6d2510:return 0x3fa0624dd2f1a9fcull;
case 0x6d25a8:return 0x400599999999999aull;
case 0x6d2618:return 0x3ff41b2f769cf0edull;
case 0x6d26e0:return 0x400ccccccccccccdull;
case 0x6d2a90:return 0x4009333333333333ull;
default:throw std::out_of_range("Stage5 first constant");}}
}
