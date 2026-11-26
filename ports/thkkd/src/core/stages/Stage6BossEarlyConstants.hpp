// 六面关底前组时间轴的精确binary64常量与原编码零终止符卡名称。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou::stage6_boss_early_detail {
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6a7708:return 0x3f847ae147ae147bull;
case 0x6aca58:return 0xbff0000000000000ull;
case 0x6b25f8:return 0x403e000000000000ull;
case 0x6d1880:return 0x3fe999999999999aull;
case 0x6d18f0:return 0x3feccccccccccccdull;
case 0x6d1900:return 0x3fc999999999999aull;
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d1950:return 0x3f50624dd2f1a9fcull;
case 0x6d1968:return 0x3f9eb851eb851eb8ull;
case 0x6d19d8:return 0x3fe6666666666666ull;
case 0x6d1a28:return 0x4078800000000000ull;
case 0x6d1a30:return 0xc020000000000000ull;
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6d1a48:return 0x407c800000000000ull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1a68:return 0x407d000000000000ull;
case 0x6d1a78:return 0x4079000000000000ull;
case 0x6d1a90:return 0x4054000000000000ull;
case 0x6d1ae8:return 0x4044000000000000ull;
case 0x6d1af0:return 0x406c000000000000ull;
case 0x6d1b08:return 0x3fef0a3d70a3d70aull;
case 0x6d1b18:return 0x4004000000000000ull;
case 0x6d1b58:return 0xc03e000000000000ull;
case 0x6d1bb8:return 0x4079e00000000000ull;
case 0x6d1bd0:return 0x3fefae147ae147aeull;
case 0x6d1bf0:return 0x4080800000000000ull;
case 0x6d1bf8:return 0xc054000000000000ull;
case 0x6d1c18:return 0x4010000000000000ull;
case 0x6d1c60:return 0x40b88b0000000000ull;
case 0x6d1d50:return 0x4051800000000000ull;
case 0x6d1d68:return 0x407de00000000000ull;
case 0x6d1e20:return 0x4030000000000000ull;
case 0x6d1e48:return 0x4040000000000000ull;
case 0x6d1f08:return 0x4061800000000000ull;
case 0x6d1f98:return 0x407ac00000000000ull;
case 0x6d2138:return 0x4000c152382d7370ull;
case 0x6d2160:return 0x4076c00000000000ull;
case 0x6d2170:return 0x4066200000000000ull;
case 0x6d2178:return 0xbf9eb851eb851eb8ull;
case 0x6d2180:return 0x3ffe666666666666ull;
case 0x6d2188:return 0x40af400000000000ull;
case 0x6d21e0:return 0x3f7cac083126e979ull;
case 0x6d21f0:return 0x4060400000000000ull;
case 0x6d2290:return 0x3fe657184ae74495ull;
case 0x6d25a8:return 0x400599999999999aull;
case 0x6d2a50:return 0xbfeccccccccccccdull;
case 0x6d31a8:return 0x3fdeeebf2ca2adbbull;
default:throw std::out_of_range("Stage5 first constant");}}
}
