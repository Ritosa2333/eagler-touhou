// EX后道中时间轴的精确binary64常量与原编码零终止符卡名称。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou::ex_third_detail {
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6a6100:return 0x0000000000000000ull;
case 0x6a7708:return 0x3f847ae147ae147bull;
case 0x6aca58:return 0xbff0000000000000ull;
case 0x6b25b8:return 0x4059000000000000ull;
case 0x6b25f8:return 0x403e000000000000ull;
case 0x6d1830:return 0x407b000000000000ull;
case 0x6d1848:return 0xc044000000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d1880:return 0x3fe999999999999aull;
case 0x6d1898:return 0x4024000000000000ull;
case 0x6d18f0:return 0x3feccccccccccccdull;
case 0x6d1910:return 0x4014000000000000ull;
case 0x6d1918:return 0x3fa999999999999aull;
case 0x6d1920:return 0x3fd3333333333333ull;
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d1950:return 0x3f50624dd2f1a9fcull;
case 0x6d1960:return 0x3fb999999999999aull;
case 0x6d1968:return 0x3f9eb851eb851eb8ull;
case 0x6d1980:return 0x3fedc28f5c28f5c3ull;
case 0x6d19a0:return 0x407c000000000000ull;
case 0x6d19c0:return 0x3fe0000000000000ull;
case 0x6d19c8:return 0x3fcd70a3d70a3d71ull;
case 0x6d19e8:return 0x4079400000000000ull;
case 0x6d19f0:return 0xc034000000000000ull;
case 0x6d19f8:return 0x407d400000000000ull;
case 0x6d1a20:return 0x3fee666666666666ull;
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6d1a48:return 0x407c800000000000ull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1a98:return 0x4007333333333333ull;
case 0x6d1ab0:return 0x4069000000000000ull;
case 0x6d1ad0:return 0x3ff8000000000000ull;
case 0x6d1ad8:return 0x4008000000000000ull;
case 0x6d1ae8:return 0x4044000000000000ull;
case 0x6d1af8:return 0x4072c00000000000ull;
case 0x6d1b08:return 0x3fef0a3d70a3d70aull;
case 0x6d1b18:return 0x4004000000000000ull;
case 0x6d1b58:return 0xc03e000000000000ull;
case 0x6d1b90:return 0x4039000000000000ull;
case 0x6d1bb8:return 0x4079e00000000000ull;
case 0x6d1bd0:return 0x3fefae147ae147aeull;
case 0x6d1c00:return 0x407e800000000000ull;
case 0x6d1c08:return 0x407a800000000000ull;
case 0x6d1c18:return 0x4010000000000000ull;
case 0x6d1c30:return 0x407b200000000000ull;
case 0x6d1c90:return 0x400c000000000000ull;
case 0x6d1cb0:return 0x4078000000000000ull;
case 0x6d1ce0:return 0x4006666666666666ull;
case 0x6d1d28:return 0x3f60624dd2f1a9fcull;
case 0x6d1d50:return 0x4051800000000000ull;
case 0x6d1d58:return 0x400e666666666666ull;
case 0x6d1d60:return 0x400a666666666666ull;
case 0x6d1d68:return 0x407de00000000000ull;
case 0x6d1de0:return 0xbff921fb54442d28ull;
case 0x6d1e78:return 0x4041800000000000ull;
case 0x6d2028:return 0x407f400000000000ull;
case 0x6d20e0:return 0x407b600000000000ull;
case 0x6d20e8:return 0x4077600000000000ull;
case 0x6d2138:return 0x4000c152382d7370ull;
case 0x6d2140:return 0x3fe0c152382d7370ull;
case 0x6d2148:return 0x3fef6c8b43958106ull;
case 0x6d2150:return 0x400999999999999aull;
case 0x6d2158:return 0x3fd657184ae74495ull;
case 0x6d2160:return 0x4076c00000000000ull;
case 0x6d2178:return 0xbf9eb851eb851eb8ull;
case 0x6d21f8:return 0xbff999999999999aull;
case 0x6d2478:return 0xc00b333333333333ull;
case 0x6d2480:return 0x400b333333333333ull;
case 0x6d2488:return 0x3ff4cccccccccccdull;
case 0x6d2490:return 0x405fc00000000000ull;
case 0x6d25f0:return 0x4004cccccccccccdull;
case 0x6d29b0:return 0x3fc5c28f5c28f5c3ull;
default:throw std::out_of_range("EX third constant");}}
}
