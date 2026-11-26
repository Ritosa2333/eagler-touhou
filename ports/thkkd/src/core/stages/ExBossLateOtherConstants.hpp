#pragma once
#include <cstdint>
#include <stdexcept>
namespace kaikeidou { namespace ex_boss_late_other_detail {
inline constexpr std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6a6100:return 0x0000000000000000ull;
case 0x6b25f8:return 0x403e000000000000ull;
case 0x6d1840:return 0xc040000000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d1898:return 0x4024000000000000ull;
case 0x6d18c8:return 0x407e000000000000ull;
case 0x6d18e8:return 0x4020000000000000ull;
case 0x6d1900:return 0x3fc999999999999aull;
case 0x6d1918:return 0x3fa999999999999aull;
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d1950:return 0x3f50624dd2f1a9fcull;
case 0x6d1998:return 0x4080000000000000ull;
case 0x6d19a0:return 0x407c000000000000ull;
case 0x6d19a8:return 0xc050000000000000ull;
case 0x6d1a08:return 0x3f947ae147ae147bull;
case 0x6d1a38:return 0x407a000000000000ull;
case 0x6d1a48:return 0x407c800000000000ull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1a68:return 0x407d000000000000ull;
case 0x6d1a70:return 0xc12e847e00000000ull;
case 0x6d1a78:return 0x4079000000000000ull;
case 0x6d1a80:return 0xc030000000000000ull;
case 0x6d1a98:return 0x4007333333333333ull;
case 0x6d1aa0:return 0x3f789374bc6a7efaull;
case 0x6d1aa8:return 0x404e000000000000ull;
case 0x6d1ab0:return 0x4069000000000000ull;
case 0x6d1ab8:return 0x3f80624dd2f1a9fcull;
case 0x6d1ac0:return 0x3feff7ced916872bull;
case 0x6d1ad0:return 0x3ff8000000000000ull;
case 0x6d1ae0:return 0x40c1940000000000ull;
case 0x6d1af0:return 0x406c000000000000ull;
case 0x6d1b18:return 0x4004000000000000ull;
case 0x6d1b48:return 0x3f8eb851eb851eb8ull;
case 0x6d1b88:return 0x4028000000000000ull;
case 0x6d1b90:return 0x4039000000000000ull;
case 0x6d1b98:return 0x3fe45a1cac083127ull;
case 0x6d1c60:return 0x40b88b0000000000ull;
case 0x6d1c80:return 0x3ffb333333333333ull;
case 0x6d1c90:return 0x400c000000000000ull;
case 0x6d1cb0:return 0x4078000000000000ull;
case 0x6d1de0:return 0xbff921fb54442d28ull;
case 0x6d1e20:return 0x4030000000000000ull;
case 0x6d1ea8:return 0x4012d97c7f3321deull;
case 0x6d1f00:return 0xc01f6a7a29553872ull;
case 0x6d1f08:return 0x4061800000000000ull;
case 0x6d1ff8:return 0x3ffccccccccccccdull;
case 0x6d2140:return 0x3fe0c152382d7370ull;
case 0x6d21c8:return 0x3f826e978d4fdf3bull;
case 0x6d2290:return 0x3fe657184ae74495ull;
case 0x6d22d8:return 0x3fe921fb54442d28ull;
case 0x6d2350:return 0xbf947ae147ae147bull;
case 0x6d2488:return 0x3ff4cccccccccccdull;
case 0x6d27b0:return 0x3fe247426bd47defull;
case 0x6d2828:return 0x3f889374bc6a7efaull;
case 0x6d2970:return 0x3fecb91f3bbba152ull;
case 0x6d2d60:return 0x3fd52a170397ba3dull;
default:throw std::out_of_range("EX heavy constant");}}
}}
