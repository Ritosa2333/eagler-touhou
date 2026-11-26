// 原三面后半段时间轴读取的binary64常量。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou::stage3_third_detail {
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6b25b8:return 0x4059000000000000ull;
case 0x6d1868:return 0x3ff0000000000000ull;
case 0x6d1950:return 0x3f50624dd2f1a9fcull;
case 0x6d19f0:return 0xc034000000000000ull;
case 0x6d1a00:return 0x3ff3333333333333ull;
case 0x6d1a18:return 0x3fb47ae147ae147bull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1aa8:return 0x404e000000000000ull;
case 0x6d1ad8:return 0x4008000000000000ull;
case 0x6d1c60:return 0x40b88b0000000000ull;
case 0x6d1ce0:return 0x4006666666666666ull;
case 0x6d1d78:return 0x4060000000000000ull;
case 0x6d1f50:return 0x3ff199999999999aull;
case 0x6d1fc0:return 0xbff4cccccccccccdull;
case 0x6d1ff8:return 0x3ffccccccccccccdull;
case 0x6d2208:return 0x401921fb54442d28ull;
case 0x6d2218:return 0xc006666666666666ull;
case 0x6d2240:return 0x4065400000000000ull;
case 0x6d2448:return 0xbfb47ae147ae147bull;
case 0x6d2488:return 0x3ff4cccccccccccdull;
case 0x6d2670:return 0xc070000000000000ull;
default:throw std::out_of_range("Stage3 third constant");}}
}
