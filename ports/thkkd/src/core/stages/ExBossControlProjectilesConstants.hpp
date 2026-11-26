// EX后道中时间轴的精确binary64常量与原编码零终止符卡名称。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou::ex_boss_control_projectiles_detail {
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6b25b8:return 0x4059000000000000ull;
case 0x6d1950:return 0x3f50624dd2f1a9fcull;
case 0x6d1960:return 0x3fb999999999999aull;
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6d1a88:return 0x4049000000000000ull;
case 0x6d1a90:return 0x4054000000000000ull;
case 0x6d1ab0:return 0x4069000000000000ull;
case 0x6d1ae8:return 0x4044000000000000ull;
case 0x6d1c60:return 0x40b88b0000000000ull;
case 0x6d2028:return 0x407f400000000000ull;
case 0x6d2128:return 0x3f989374bc6a7efaull;
case 0x6d2488:return 0x3ff4cccccccccccdull;
case 0x6d27f8:return 0x3fd921fb54442d28ull;
default:throw std::out_of_range("EX third constant");}}
}
