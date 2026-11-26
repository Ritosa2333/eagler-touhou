// 五面后半段时间轴的精确binary64常量与原编码零终止符卡名称。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou::stage5_third_detail {
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6a6100:return 0x0000000000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d1920:return 0x3fd3333333333333ull;
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d1950:return 0x3f50624dd2f1a9fcull;
case 0x6d19a0:return 0x407c000000000000ull;
case 0x6d19c0:return 0x3fe0000000000000ull;
case 0x6d19d0:return 0x407a400000000000ull;
case 0x6d19e0:return 0x3f70624dd2f1a9fcull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1aa0:return 0x3f789374bc6a7efaull;
case 0x6d1aa8:return 0x404e000000000000ull;
case 0x6d1ad8:return 0x4008000000000000ull;
case 0x6d1af0:return 0x406c000000000000ull;
case 0x6d1b58:return 0xc03e000000000000ull;
case 0x6d1b60:return 0x3ff921fb54442d28ull;
case 0x6d1bb8:return 0x4079e00000000000ull;
case 0x6d1c60:return 0x40b88b0000000000ull;
case 0x6d1cb0:return 0x4078000000000000ull;
case 0x6d1ce8:return 0x4002666666666666ull;
case 0x6d1cf8:return 0x3f689374bc6a7efaull;
case 0x6d1d68:return 0x407de00000000000ull;
case 0x6d1de0:return 0xbff921fb54442d28ull;
case 0x6d1e48:return 0x4040000000000000ull;
case 0x6d2138:return 0x4000c152382d7370ull;
case 0x6d2180:return 0x3ffe666666666666ull;
case 0x6d2208:return 0x401921fb54442d28ull;
case 0x6d22d8:return 0x3fe921fb54442d28ull;
case 0x6d25e8:return 0xbfd3333333333333ull;
case 0x6d25f0:return 0x4004cccccccccccdull;
case 0x6d2618:return 0x3ff41b2f769cf0edull;
default:throw std::out_of_range("Stage5 first constant");}}
}
