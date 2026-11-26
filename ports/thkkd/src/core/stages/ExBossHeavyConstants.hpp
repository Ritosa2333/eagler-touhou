#pragma once
#include <cstdint>
#include <stdexcept>
namespace kaikeidou { namespace ex_boss_heavy_detail {
inline constexpr std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6a6100:return 0x0000000000000000ull;
case 0x6b25b8:return 0x4059000000000000ull;
case 0x6b25f8:return 0x403e000000000000ull;
case 0x6d1858:return 0x4066800000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d1868:return 0x3ff0000000000000ull;
case 0x6d1900:return 0x3fc999999999999aull;
case 0x6d1908:return 0x4018000000000000ull;
case 0x6d1910:return 0x4014000000000000ull;
case 0x6d1920:return 0x3fd3333333333333ull;
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d1960:return 0x3fb999999999999aull;
case 0x6d1990:return 0x3fc1eb851eb851ecull;
case 0x6d19b8:return 0x3fc70a3d70a3d70aull;
case 0x6d19c0:return 0x3fe0000000000000ull;
case 0x6d1a68:return 0x407d000000000000ull;
case 0x6d1a78:return 0x4079000000000000ull;
case 0x6d1a80:return 0xc030000000000000ull;
case 0x6d1a88:return 0x4049000000000000ull;
case 0x6d1a90:return 0x4054000000000000ull;
case 0x6d1aa8:return 0x404e000000000000ull;
case 0x6d1ab0:return 0x4069000000000000ull;
case 0x6d1ab8:return 0x3f80624dd2f1a9fcull;
case 0x6d1ac8:return 0x406a400000000000ull;
case 0x6d1ad0:return 0x3ff8000000000000ull;
case 0x6d1ad8:return 0x4008000000000000ull;
case 0x6d1ae8:return 0x4044000000000000ull;
case 0x6d1af8:return 0x4072c00000000000ull;
case 0x6d1b38:return 0x4056800000000000ull;
case 0x6d1d50:return 0x4051800000000000ull;
case 0x6d1e50:return 0x408f400000000000ull;
case 0x6d1f10:return 0x3f816872b020c49cull;
case 0x6d1f18:return 0xbf841205bc01a36eull;
case 0x6d1f20:return 0x4092c00000000000ull;
case 0x6d1f28:return 0x3fcef9db22d0e560ull;
case 0x6d1f30:return 0x4057c00000000000ull;
case 0x6d1f38:return 0x4099000000000000ull;
case 0x6d1f40:return 0x401ccccccccccccdull;
case 0x6d1f48:return 0x3fb916872b020c4aull;
case 0x6d1f50:return 0x3ff199999999999aull;
case 0x6d1f58:return 0x4014cccccccccccdull;
case 0x6d1f60:return 0x4071800000000000ull;
case 0x6d1f68:return 0x4013333333333333ull;
case 0x6d1f70:return 0x4076800000000000ull;
case 0x6d1f78:return 0x3fe199999999999aull;
case 0x6d1f80:return 0x3fd851eb851eb852ull;
case 0x6d1f88:return 0x3fdae147ae147ae1ull;
case 0x6d1f90:return 0x3fdd70a3d70a3d71ull;
default:throw std::out_of_range("EX heavy constant");}}
}}
