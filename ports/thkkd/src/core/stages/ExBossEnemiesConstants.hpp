// EX后道中时间轴的精确binary64常量与原编码零终止符卡名称。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou::ex_boss_enemies_detail {
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6b2620:return 0x3f747ae147ae147bull;
case 0x6d18c0:return 0x4064000000000000ull;
case 0x6d1940:return 0x3fef5c28f5c28f5cull;
case 0x6d1950:return 0x3f50624dd2f1a9fcull;
case 0x6d1a68:return 0x407d000000000000ull;
case 0x6d1a78:return 0x4079000000000000ull;
case 0x6d1a80:return 0xc030000000000000ull;
case 0x6d1ad0:return 0x3ff8000000000000ull;
case 0x6d1c18:return 0x4010000000000000ull;
case 0x6d1c60:return 0x40b88b0000000000ull;
case 0x6d1c90:return 0x400c000000000000ull;
case 0x6d2118:return 0x3fb657184ae74495ull;
case 0x6d2120:return 0x3fc32614d27d1637ull;
case 0x6d2128:return 0x3f989374bc6a7efaull;
case 0x6d2130:return 0x4000cccccccccccdull;
case 0x6d22d8:return 0x3fe921fb54442d28ull;
case 0x6d25f0:return 0x4004cccccccccccdull;
case 0x6d27b0:return 0x3fe247426bd47defull;
case 0x6d27f8:return 0x3fd921fb54442d28ull;
case 0x6d2ee8:return 0x3fd247426bd47defull;
default:throw std::out_of_range("EX third constant");}}
}
